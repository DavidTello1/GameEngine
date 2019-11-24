#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ResourceScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentRenderer.h"
#include "ComponentCamera.h"

#include "SimpleBinStream.h"

ResourceScene::ResourceScene(UID id) : Resource(id, Resource::Type::scene)
{
}

ResourceScene::~ResourceScene()
{
}

bool ResourceScene::Import(const char* full_path, std::string& output)
{
	ResourceScene scene(0);

	bool ret = scene.SaveOwnFormat(output);

	if (ret)
	{
		scene.original_file = full_path; //get file
		App->file_system->NormalizePath(scene.original_file);

		std::string file_name = App->file_system->GetFileName(output.c_str());//get exported file
		scene.exported_file = file_name;

		LOG("Imported scene from [%s] to [%s]", scene.GetFile(), scene.GetExportedFile());
	}
	else
	{
		LOG("Importing scene %s FAILED", full_path);
	}

	scene.LoadToMemory(); // HARDCODED : should be called when loading after importing (just for testing)

	return ret;
}

bool ResourceScene::SaveOwnFormat(std::string& output) const
{
	simple::mem_ostream<std::true_type> write_stream; //create output stream

	write_stream << uint(App->scene->gameObjects.size()); // save gameobjects size

	for (uint i = 0; i < App->scene->gameObjects.size(); ++i) // save data
	{
		write_stream << App->scene->gameObjects[i]->GetName();
		write_stream << App->scene->gameObjects[i]->GetUID();
		write_stream << App->scene->gameObjects[i]->parent->GetUID();
		write_stream << App->scene->gameObjects[i]->components.size();

		if (App->scene->gameObjects[i]->HasComponent(Component::Type::Mesh))
		{
			ComponentMesh* mesh = (ComponentMesh*)App->scene->gameObjects[i]->GetComponent(Component::Type::Mesh);
			write_stream << mesh->type;
			write_stream << mesh->GetMesh()->GetID();
		}
		if (App->scene->gameObjects[i]->HasComponent(Component::Type::Material))
		{
			ComponentMaterial* material = (ComponentMaterial*)App->scene->gameObjects[i]->GetComponent(Component::Type::Material);
			write_stream << material->type;
			write_stream << material->GetMaterial()->GetID();
		}
		if (App->scene->gameObjects[i]->HasComponent(Component::Type::Renderer))
		{
			ComponentRenderer* renderer = (ComponentRenderer*)App->scene->gameObjects[i]->GetComponent(Component::Type::Renderer);
			write_stream << renderer->type;
		}
		if (App->scene->gameObjects[i]->HasComponent(Component::Type::Camera))
		{
			ComponentCamera* camera = (ComponentCamera*)App->scene->gameObjects[i]->GetComponent(Component::Type::Camera);
			write_stream << camera->type;
			//Add important variables
		}

		write_stream << App->scene->gameObjects[i]->IsActive();
		write_stream << App->scene->gameObjects[i]->GetPosition();
		write_stream << App->scene->gameObjects[i]->GetRotation();
		write_stream << App->scene->gameObjects[i]->GetScale();
		write_stream << App->scene->gameObjects[i]->is_static;
	}

	const std::vector<char>& data = write_stream.get_internal_vec(); //get stream vector

	return App->file_system->SaveUnique(output, &data[0], data.size(), LIBRARY_SCENE_FOLDER, std::to_string(GetID()).c_str(), "dvs"); //save file
}

bool ResourceScene::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		uint size = App->file_system->LoadFromPath(LIBRARY_SCENE_FOLDER, GetExportedFile(), &buffer); //get total size

		simple::mem_istream<std::true_type> read_stream(buffer, size); //create input stream

		uint objs_size = 0;
		read_stream >> objs_size; //get node size
		gameobjs.reserve(objs_size); //reserve memory in vector for size

		for (uint i = 0; i < objs_size; ++i) // load data
		{
			GameObject* obj = App->scene->CreateGameObject();
			const char* name_;
			UID id_;
			bool active_;
			uint size_;

			read_stream >> name_;
			read_stream >> obj->parent;
			read_stream >> id_;
			read_stream >> size_;

			obj->SetUID(id_);
			obj->SetName(name_);
			obj->components.reserve(size_);

			for (uint j = 0; j < size; ++j)
			{
				Component::Type type;
				UID res_id;

				read_stream >> type;

				if (type == Component::Type::Mesh)
				{
					read_stream >> res_id;

					ComponentMesh* mesh = (ComponentMesh*)obj->AddComponent(Component::Type::Mesh);
					ResourceMesh* r_mesh = (ResourceMesh*)App->resources->GetResource(res_id);
					r_mesh->LoadToMemory(); //load mesh data
					mesh->SetMesh(r_mesh);

					obj->GenerateBoundingBox();
				}

				if (type == Component::Type::Material)
				{
					read_stream >> res_id;

					ComponentMaterial* material = (ComponentMaterial*)obj->AddComponent(Component::Type::Material);
					ResourceMaterial* r_material = (ResourceMaterial*)App->resources->GetResource(res_id);
					r_material->LoadToMemory(); //load material data
					material->SetMaterial(r_material);
				}

				if (type == Component::Type::Material)
				{
					obj->AddComponent(Component::Type::Renderer);
				}

				if (type == Component::Type::Camera)
				{
					ComponentCamera* camera = (ComponentCamera*)obj->AddComponent(Component::Type::Camera);
				}
			}

			App->scene->gameObjects.push_back(obj); //add gameobj to main scene
		}
		return true;
	}
	return false;
}

void ResourceScene::UnLoad()
{
	LOG("UnLoading Scene %s with uid %d", name.c_str(), uid, 'd');
	for (uint i = 0; i < gameobjs.size(); ++i)
	{
		RELEASE(gameobjs[i]);
	}
	gameobjs.clear();
}