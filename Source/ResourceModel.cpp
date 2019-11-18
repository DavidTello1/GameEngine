#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ResourceModel.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"

#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include "SimpleBinStream.h"

#include "Assimp/include/scene.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"


ResourceModel::ResourceModel(UID id) : Resource(id, Resource::Type::model)
{
}

ResourceModel::~ResourceModel()
{
}

//void ResourceModel::Save(Config& config) const
//{
//	Resource::Save(config);
//}
//
//void ResourceModel::Load(const Config& config)
//{
//	Resource::Load(config);
//}

bool ResourceModel::Import(const char* full_path, std::string& output)
{
	unsigned flags = aiProcess_CalcTangentSpace | \
		aiProcess_GenSmoothNormals | \
		aiProcess_JoinIdenticalVertices | \
		aiProcess_ImproveCacheLocality | \
		aiProcess_LimitBoneWeights | \
		aiProcess_SplitLargeMeshes | \
		aiProcess_Triangulate | \
		aiProcess_GenUVCoords | \
		aiProcess_SortByPType | \
		aiProcess_FindDegenerates | \
		aiProcess_FindInvalidData |
		0;

	aiString assimp_path(".");
	assimp_path.Append(full_path);

	const aiScene* scene = aiImportFile(assimp_path.data, flags);
	if (scene)
	{
		ResourceModel model(0);
		std::vector<UID> materials, meshes;

		// Generate Materials
		materials.reserve(scene->mNumMaterials); //reserve capacity for num of materials
		for (unsigned i = 0; i < scene->mNumMaterials; ++i)
		{
			materials.push_back(ResourceMaterial::Import(scene->mMaterials[i], full_path)); //import material

			assert(materials.back() != 0); //asssert if vector materials is empty
		}

		// Generate Meshes
		meshes.reserve(scene->mNumMeshes);
		for (unsigned i = 0; i < scene->mNumMeshes; ++i)
		{
			meshes.push_back(ResourceMesh::Import(scene->mMeshes[i], full_path));

			assert(meshes.back() != 0);
		}

		// Generate GameObjects
		model.CreateNodes(scene, scene->mRootNode, 0, meshes, materials);

		aiReleaseImport(scene);

		bool ret = model.SaveOwnFormat(output);

		if (ret)
		{
			model.file = full_path; //get file
			App->file_system->NormalizePath(model.file);

			std::string file_name; //get exported file
			App->file_system->SplitFilePath(output.c_str(), nullptr, &file_name);
			model.exported_file = file_name;

			LOG("Imported aiScene from [%s] to [%s]", model.GetFile(), model.GetExportedFile());
		}
		else
		{
			LOG("Importing aiScene %s FAILED", full_path);
		}

		model.CreateGameObjects(meshes, materials);

		return ret;
	}
	return false;
}

bool ResourceModel::SaveOwnFormat(std::string& output) const
{
	simple::mem_ostream<std::true_type> write_stream; //create output stream

	write_stream << uint(nodes.size()); // save nodes size

	for (uint i = 0; i < nodes.size(); ++i) // save data
	{
		write_stream << nodes[i].name;
		write_stream << nodes[i].parent;
		write_stream << nodes[i].mesh;
		write_stream << nodes[i].material;
	}

	const std::vector<char>& data = write_stream.get_internal_vec(); //get stream vector

	return App->file_system->SaveUnique(output, &data[0], data.size(), LIBRARY_MODEL_FOLDER, "model", "dvs_model"); //save file
}

bool ResourceModel::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		uint size = App->file_system->Load(LIBRARY_MODEL_FOLDER, GetExportedFile(), &buffer); //get total size

		simple::mem_istream<std::true_type> read_stream(buffer, size); //create input stream

		uint node_size = 0;
		read_stream >> node_size; //get node size
		nodes.reserve(node_size); //reserve memory in vector for size

		for (uint i = 0; i < node_size; ++i) // load data
		{
			Node node;

			read_stream >> node.name;
			read_stream >> node.parent;
			read_stream >> node.mesh;
			read_stream >> node.material;

			nodes.push_back(node); //add node to vector nodes
		}

		for (uint i = 0; i < nodes.size(); ++i) //load meshes and materials
		{
			if (nodes[i].mesh != 0)
				App->resources->GetResource(nodes[i].mesh)->LoadToMemory();

			if (nodes[i].material != 0)
				App->resources->GetResource(nodes[i].material)->LoadToMemory();
		}
		return true;
	}
	return false;
}

void ResourceModel::UnLoad()
{
	LOG("UnLoading Model %s with uid %d", name, uid, 'd');
	for (uint i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].mesh != 0)
			App->resources->GetResource(nodes[i].mesh)->ReleaseFromMemory();

		if (nodes[i].material != 0)
			App->resources->GetResource(nodes[i].material)->ReleaseFromMemory();
	}
	nodes.clear();
}

void ResourceModel::CreateNodes(const aiScene* model, const aiNode* node, uint parent, const std::vector<UID>& meshes, const std::vector<UID>& materials)
{
	uint index = nodes.size(); //get position in vector

	// create node and init
	Node dst;
	dst.name = node->mName.C_Str();
	dst.parent = parent;
	nodes.push_back(dst); //add node to vector

	for (unsigned i = 0; i < node->mNumChildren; ++i) //check for children and create respective nodes with actual node as parent
	{
		CreateNodes(model, node->mChildren[i], index, meshes, materials);
	}

	// Check for meshes and materials and init if exist
	if (node->mNumMeshes == 1) //if there is only one
	{
		uint mesh_index = node->mMeshes[0];

		nodes[index].mesh = meshes[mesh_index];
		nodes[index].material = materials[model->mMeshes[mesh_index]->mMaterialIndex];
	}
	else
	{
		for (uint i = 0; i < node->mNumMeshes; ++i) //if there are more than one iterate
		{
			uint mesh_index = node->mMeshes[i];

			Node mesh;
			mesh.parent = index;
			mesh.mesh = meshes[mesh_index];
			mesh.material = materials[model->mMeshes[mesh_index]->mMaterialIndex];

			char buff[100];
			if (model->mMeshes[mesh_index]->mName.length == 0)
				snprintf(buff, sizeof(buff), "mesh_%d", i);
			else
				snprintf(buff, sizeof(buff), "mesh_%s", model->mMeshes[mesh_index]->mName.C_Str());

			mesh.name = buff;

			nodes.push_back(mesh);
		}
	}
}

void ResourceModel::CreateGameObjects(const std::vector<UID>& meshes, const std::vector<UID>& materials)
{
	std::vector<GameObject*> objects;
	GameObject* obj;

	for (uint i = 0; i < nodes.size(); ++i)
	{
		GameObject* parent = nullptr;

		if (i > 0)
			parent = objects[nodes[i].parent];

		obj = App->scene->CreateGameObject(nodes[i].name.c_str(), parent);
		objects.push_back(obj);

		if (nodes[i].mesh != 0)
		{
			ComponentMesh* mesh = (ComponentMesh*)obj->AddComponent(Component::Type::Mesh);
			ResourceMesh* r_mesh = (ResourceMesh*)App->resources->GetResource(nodes[i].mesh);
			mesh->SetMesh(r_mesh);
			//mesh->SetBoundingBox();
		}

		//if (nodes[i].material != 0)
		//{
		//	obj->AddComponent(Component::Type::Material);
		//}

	}

	objects.clear();
	LoadToMemory();
}