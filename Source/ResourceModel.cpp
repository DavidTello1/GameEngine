#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
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
	Timer timer;
	timer.Start();

	const aiScene* scene = aiImportFileEx(full_path, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	if (scene)
	{
		ResourceModel model(0);
		std::vector<UID> materials, meshes;

		model.name = App->file_system->GetFileName(full_path);
		model.name = model.name.substr(0, model.name.size() - 4);
		LOG("Importing model '%s'", (model.name.c_str()), 'g');


		// Generate Materials
		materials.reserve(scene->mNumMaterials); //reserve capacity for num of materials
		for (unsigned i = 0; i < scene->mNumMaterials; ++i)
		{
			std::string output2;
			materials.push_back(ResourceMaterial::Import(full_path, output2, scene->mMaterials[i])); //import material

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
		model.nodes.clear(); //clear nodes data

		if (ret)
		{
			model.original_file = full_path; //get file
			App->file_system->NormalizePath(model.original_file);

			std::string file_name = App->file_system->GetFileName(output.c_str());//get exported file
			model.exported_file = file_name;

			LOG("Imported aiScene from [%s] to [%s]", model.GetFile(), model.GetExportedFile());
		}
		else
		{
			LOG("Importing aiScene %s FAILED", full_path);
		}
		LOG("[%s] imported in %d ms", model.GetFile(), timer.Read(), 'd');
		timer.Stop();
		
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
		write_stream << nodes[i].transform;
		write_stream << nodes[i].mesh;
		write_stream << nodes[i].material;
	}

	const std::vector<char>& data = write_stream.get_internal_vec(); //get stream vector

	return App->file_system->SaveUnique(output, &data[0], data.size(), LIBRARY_MODEL_FOLDER, name.c_str(), "dvs_model"); //save file
}

bool ResourceModel::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		Timer timer;
		timer.Start();
		char* buffer = nullptr;
		uint size = App->file_system->LoadFromPath(LIBRARY_MODEL_FOLDER, GetExportedFile(), &buffer); //get total size

		simple::mem_istream<std::true_type> read_stream(buffer, size); //create input stream

		uint node_size = 0;
		read_stream >> node_size; //get node size
		nodes.reserve(node_size); //reserve memory in vector for size

		for (uint i = 0; i < node_size; ++i) // load data
		{
			Node node;

			read_stream >> node.name;
			read_stream >> node.parent;

			for (uint i = 0; i < 16; ++i)
			{
				read_stream >> reinterpret_cast<float*>(&node.transform)[i];
			}

			read_stream >> node.mesh;
			read_stream >> node.material;

			nodes.push_back(node); //add node to vector nodes
		}

		LOG("[%s] loaded in %d ms", GetExportedFile(), timer.Read(), 'd');
		timer.Stop();
		return true;
	}
	return false;
}

void ResourceModel::UnLoad()
{
	LOG("UnLoading Model %s with uid %d", name.c_str(), uid, 'd');
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

void ResourceModel::CreateGameObjects(const char* name)
{
	std::vector<GameObject*> objects;

	GameObject* obj;

	for (uint i = 0; i < nodes.size(); ++i)
	{
		GameObject* parent = nullptr;

		if (i > 0)
			parent = objects[nodes[i].parent];

		if (strcmp("RootNode", nodes[i].name.c_str()) == 0 && name != nullptr) //if RootNode init name as file name
			obj = App->scene->CreateGameObject(name, parent);
		else
			obj = App->scene->CreateGameObject(nodes[i].name.c_str(), parent);
		

		objects.push_back(obj);

		if (nodes[i].mesh != 0)
		{
			ComponentMesh* mesh = (ComponentMesh*)obj->AddComponent(Component::Type::Mesh);
			ResourceMesh* r_mesh = (ResourceMesh*)App->resources->GetResource(nodes[i].mesh);
			r_mesh->LoadToMemory(); //load mesh data
			mesh->SetMesh(r_mesh);

			obj->GenerateBoundingBox();

			obj->AddComponent(Component::Type::Renderer);
		}

		if (nodes[i].material != 0)
		{
			ComponentMaterial* material = (ComponentMaterial*)obj->AddComponent(Component::Type::Material);
			ResourceMaterial* r_material = (ResourceMaterial*)App->resources->GetResource(nodes[i].material);
			r_material->LoadToMemory(); //load material data
			material->SetMaterial(r_material);
		}
	}


	// Removing useless parents, maybe not the better algth, but works :D
	std::vector<GameObject*> childs_to_remove;
	std::vector<int> pos_to_remove;

	for (int i = 0; i < objects.size(); i++)
	{
		if (strstr(objects[i]->GetName(), "$AssimpFbx$") != nullptr)
		{
			childs_to_remove.push_back(objects[i]);
		}
		else
		{
			if (!childs_to_remove.empty())
			{
				objects[i]->parent = childs_to_remove[0]->parent;
				objects[i]->parent->childs.push_back(objects[i]);
				objects[i]->parent->flags |= ProcessNewChild;
			}

			while (!childs_to_remove.empty())
			{
				childs_to_remove.back()->childs.clear();
				App->scene->DeleteGameObject(childs_to_remove.back());
				childs_to_remove.pop_back();
			}
		}

	}

	childs_to_remove.clear();
	objects.clear();
}