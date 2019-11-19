#include "ModuleScene.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

GameObject* ModuleScene::root_object;

ModuleScene::ModuleScene(bool start_enabled) : Module("Scene", start_enabled)
{}


ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start(Config* config)
{
	LOG("Loading main scene", 'v');

	root_object = new GameObject("Root", nullptr);
	root_object->uid = 0;

	// Create game objects after this ^^^^^^^^^^^^	

	test_camera = CreateGameObject("test camera");
	test_camera->AddComponent(Component::Type::Camera);

	//GameObject* bparent = CreateGameObject("BakerHouse");

	//App->resources->LoadResource("Assets/BakerHouse.fbx", Component::Type::Mesh, true, bparent);
	//App->resources->LoadResource("Assets/Baker_house.png", Component::Type::Material, true);

	// ParShapes 
	/*GameObject* pparent = CreateGameObj("ParShapes");
	for (int i = 0; i < shape_type::UNKNOWN; i++)
	{
		App->resources->CreateShape((shape_type)i, 9, 9, i * 7.5 - 50, 2.5f, -10, 0.5f, pparent->GetUID());
	}*/

	UnSelectAll();

	return true;
}

bool ModuleScene::Update(float dt)
{
	return true;
}



bool ModuleScene::PostUpdate(float dt)
{
	// Do all remaining actions activated by flags
	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject* obj = gameObjects[i];

		if (obj->flags & ProcessNewChild)
		{
			obj->ChildAdded();
			obj->flags &= ~ProcessNewChild;
		}
		if (obj->flags & ProcessDeletedChild)
		{
			obj->ChildDeleted();
			obj->flags &= ~ProcessDeletedChild;
		}
	}

	return true;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading Main scene");

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		RELEASE(gameObjects[i]);
	}
	gameObjects.clear();

	for (uint i = 0; i < materials.size(); ++i)
	{
		if (!DeleteMaterial(materials[i]))
			RELEASE(materials[i]);
	}
	materials.clear();

	return true;
}

bool ModuleScene::Draw()
{
	// Draw GameObjects
	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		glPushMatrix();
		glMultMatrixf(gameObjects[i]->GetLocalTransform().ptr());

		ComponentRenderer* renderer = (ComponentRenderer*)gameObjects[i]->GetComponent(Component::Type::Renderer);
		if (renderer != nullptr && renderer->IsActive())
		{
			if (renderer->show_wireframe || App->scene_base->show_all_wireframe) //wireframe
			{

				glColor3ub(App->scene_base->wireframe_color.r*255.0f, App->scene_base->wireframe_color.g * 255.0f, App->scene_base->wireframe_color.b * 255.0f);
				glLineWidth(App->scene_base->wireframe_width);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			renderer->Draw();
		}

		// Bounding boxes
		GameObject* obj = gameObjects[i];
		glEnableClientState(GL_VERTEX_ARRAY);
		if ((obj->show_bounding_box || App->scene_base->show_all_bounding_box) && obj->bb_VBO != 0)
		{
			glColor3ub(App->scene_base->bounding_box_color.r * 255.0f, App->scene_base->bounding_box_color.g * 255.0f, App->scene_base->bounding_box_color.b * 255.0f);
			glLineWidth(App->scene_base->bounding_box_width);

			glBindBuffer(GL_ARRAY_BUFFER, obj->bb_VBO);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->bb_IBO);
			glDrawElements(GL_LINES, sizeof(App->resources->bbox_indices), GL_UNSIGNED_INT, nullptr);


			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glColor3ub(255, 255, 255);
		}
		glDisableClientState(GL_VERTEX_ARRAY);

		glPopMatrix();
	}
	return true;
}

GameObject* ModuleScene::FindById(uint id)
{
	for (int i = 0; i < gameObjects.size(); i++)
		if (gameObjects[i]->GetUID() == id) return gameObjects[i];

	return nullptr;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent, bool visible)
{
	create_gameobj = true;

	if (parent == nullptr) parent = root_object;

	GameObject* obj = new GameObject(name, parent);

	if (visible)
		obj->AddComponent(Component::Type::Renderer);

	gameObjects.push_back(obj);

	return obj;
}


void ModuleScene::DeleteGameObject(GameObject* obj)
{
	// If any child of the game object is the selected -> unselect
	if (selected_gameobj == obj)
		selected_gameobj = nullptr;

	// Actual deletion of gameobject
	if (!obj->HasChilds())
	{
		// Delete from scene gameobjects
		for (uint i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i] == obj)
			{
				gameObjects.erase(gameObjects.begin() + i);
				break;
			}
		}
		// Deleted from parent inside destructor
		RELEASE(obj);

	}
	// Delete childs
	else
	{
		for (int i = obj->childs.size() - 1; i >= 0; i--)
		{
			DeleteGameObject(obj->childs[i]);
		}
			
		obj->childs.clear();
		DeleteGameObject(obj);
	}
}

void ModuleScene::DeleteSelected()
{
	for (int i = gameObjects.size()-1; i >= 0; i--)
	{
		if (gameObjects[i]->is_selected) 
			DeleteGameObject(gameObjects[i]);
		
	}
}

void ModuleScene::UnSelectAll(GameObject* keep_selected)
{
	if (keep_selected == nullptr) {

		for (GameObject* i : gameObjects)
		{
			i->UnSelect();
		}
	}
	else {

		for (GameObject* i : gameObjects)
		{
			if (i->GetUID() == keep_selected->GetUID())
				continue;
			i->UnSelect();
		}
	}

}
bool ModuleScene::IsMaterialLoaded(const char* path)
{
	for (uint i = 0; i < materials.size(); i++)
	{
		if (strcmp(materials[i]->path, path) == 0)
			return true;
	}
	return false;
}

ComponentMaterial* ModuleScene::GetMaterial(const char* path) const
{
	for (uint i = 0; i < materials.size(); ++i)
	{
		if (strcmp(materials[i]->path, path) == 0)
			return materials[i];
	}
	return nullptr;
}

bool ModuleScene::DeleteMaterial(ComponentMaterial* material)
{
	for (uint i = 0; i < materials.size(); i++)
	{
		if (materials[i] == material)
		{
			materials.erase(materials.begin() + i);
			return true;
		}
	}
	return false;
}

