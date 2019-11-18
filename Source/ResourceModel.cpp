#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ResourceModel.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"

#include "ComponentMesh.h"
#include "ComponentMaterial.h"

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
		model.CreateGameObjects(meshes, materials);

		aiReleaseImport(scene);

		return model.SaveOwnFormat(output);
	}
	return false;
}

bool ResourceModel::SaveOwnFormat(std::string& output) const
{
	uint size = (sizeof(std::string) + sizeof(uint) + sizeof(UID) + sizeof(UID)) * nodes.size();
	char* data = new char[size]; // Allocate
	char* cursor = data;

	for (uint i = 0; i < nodes.size(); ++i)
	{
		// Store name
		uint bytes = sizeof(std::string);
		memcpy(cursor, &nodes[i].name, bytes);

		// Store parent
		cursor += bytes;
		bytes = sizeof(uint);
		memcpy(cursor, &nodes[i].parent, bytes);

		// Store mesh
		cursor += bytes;
		bytes = sizeof(UID);
		memcpy(cursor, &nodes[i].mesh, bytes);

		// Store material
		cursor += bytes;
		bytes = sizeof(UID);
		memcpy(cursor, &nodes[i].material, bytes);
	}
	delete[] data;

	bool ret = App->file_system->SaveUnique(output, &data, size, LIBRARY_MODEL_FOLDER, "model", "dvs_model");

	return ret;
}

bool ResourceModel::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		uint size = App->file_system->Load(LIBRARY_MODEL_FOLDER, GetExportedFile(), &buffer);
		char* cursor = buffer;

		uint node_size = sizeof(std::string) + sizeof(uint) + sizeof(UID) + sizeof(UID);
		for (uint i = 0; i < node_size; ++i)
		{
			Node node;

			//// Load name
			//uint bytes = sizeof(std::string);
			//node.name = new std::string;
			//memcpy(cursor, node.name, bytes);

			//// Load parent
			//cursor += bytes;
			//bytes = sizeof(uint);
			//node.parent = new uint;
			//memcpy(cursor, &node.parent, bytes);

			//// Load mesh
			//cursor += bytes;
			//bytes = sizeof(UID);
			//node.mesh = new UID;
			//memcpy(cursor, &node.mesh, bytes);

			//// Load material
			//cursor += bytes;
			//bytes = sizeof(UID);
			//node.material = new UID;
			//memcpy(cursor, &node.material, bytes);

			nodes.push_back(node); //add node to list
		}

		// Load meshes and materials to Memory
		for (uint i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].mesh != 0)
				App->resources->GetResource(nodes[i].mesh)->LoadToMemory();

			if (nodes[i].material != 0)
				App->resources->GetResource(nodes[i].material)->LoadToMemory();
		}

		RELEASE(buffer);
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

	if (node->mNumMeshes == 1) //check for meshes and materials and init if exist
	{
		uint mesh_index = node->mMeshes[0];

		nodes[index].mesh = meshes[mesh_index];
		nodes[index].material = materials[model->mMeshes[mesh_index]->mMaterialIndex];
	}
	else
	{
		for (uint i = 0; i < node->mNumMeshes; ++i)
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
		if (i == 0)
			obj = App->scene->CreateGameObject(nodes[0].name.c_str()); //create root gameobject
		else
			obj = App->scene->CreateGameObject(nodes[i].name.c_str(), objects[nodes[i].parent]);

		objects.push_back(obj);

		if (nodes[i].mesh != 0)
		{
			ComponentMesh* mesh = (ComponentMesh*)obj->AddComponent(Component::Type::Mesh);
			ResourceMesh* r_mesh = (ResourceMesh*)App->resources->GetResource(meshes[i - 1]);
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