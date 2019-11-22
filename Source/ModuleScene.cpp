#include "ModuleScene.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ResourceModel.h"
#include "Quadtree.h"

#include "mmgr/mmgr.h"

GameObject* ModuleScene::root_object;

ModuleScene::ModuleScene(bool start_enabled) : Module("Scene", start_enabled)
{}


ModuleScene::~ModuleScene()
{
	delete root_object;
}

bool ModuleScene::Init(Config* config)
{
	root_object = new GameObject("Root", nullptr);
	root_object->uid = 0;
	// Create game objects after this ^^^^^^^^^^^^	

	return true;
}
// Load assets
bool ModuleScene::Start(Config* config)
{
	LOG("Loading main scene", 'v');

	GameObject* go = CreateGameObject("test camera");
	go->AddComponent(Component::Type::Camera);

	test_camera = go->GetComponent<ComponentCamera>();
	
	//ResourceModel* tcmodel = new ResourceModel(root_object->GetUID());
	//std::string tmdp = "MyCamera.dvs";
	//tcmodel->Import("/Assets/camera_mesh.fbx", tmdp);

	ResourceModel* bhmodel = new ResourceModel(root_object->GetUID());
	std::string tmp = "MyBakerHouse.dvs";
	bhmodel->Import("/Assets/BakerHouse.fbx",tmp);

	quadtree = new Quadtree(AABB({ -20,-20,-20 }, { 20,20,20 }));

	gameObjects[2]->SetLocalPosition({ 10,0,-10 });
	
	//quadtree->AddGameObject(bhmodel.)
	UnSelectAll();

	return true;
}

bool ModuleScene::Update(float dt)
{
	/*if (test_camera)
		test_camera->DrawFrustum();*/

	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		for (GameObject* obj : gameObjects)
		{
			//obj->UpdateBoundingBox();
			quadtree->RemoveGameObject(obj);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		for (GameObject* obj : gameObjects)
		{
			//obj->UpdateBoundingBox();
			quadtree->AddGameObject(obj);
		}
	}

	quadtree->Draw();
	return true;
}



bool ModuleScene::PostUpdate(float dt)
{
	// Do all remaining actions activated by flags
	for (uint i = 0; i < gameObjects.size(); i++)
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
		if (obj->flags & ProcessTransformUpdate)
		{
			obj->UpdateTransform();
			obj->parent->UpdateBoundingBox();
			obj->flags &= ~ProcessTransformUpdate;
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
	for (GameObject* obj : gameObjects)
	{
		// Just boxes colors things
		Color c = (!obj->HasChilds())?App->scene_base->aabb_color : Cyan;
		bool is_drawn = true;

		if (App->scene_base->camera_culling)
		{
			if (test_camera->ContainsAABB(obj->aabb))
			{
				glPushMatrix();
				glMultMatrixf(obj->GetGlobalTransform().Transposed().ptr());

				ComponentRenderer* renderer = obj->GetComponent<ComponentRenderer>();
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
				glPopMatrix();
			}
			else
			{
				c = LightGrey;
				is_drawn = false;
			}
		}
		glEnableClientState(GL_VERTEX_ARRAY);

		// AABB
		if ((obj->show_aabb || App->scene_base->show_all_aabb) && obj->aabb_VBO != 0)
		{
			glColor3ub(c.r * 255.0f, c.g * 255.0f, c.b * 255.0f);
			glLineWidth(App->scene_base->aabb_width);

			glBindBuffer(GL_ARRAY_BUFFER, obj->aabb_VBO);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabb_IBO);
			glDrawElements(GL_LINES, sizeof(aabb_indices), GL_UNSIGNED_INT, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glColor3ub(255, 255, 255);
		}
		// OBB
		if ((obj->show_obb || App->scene_base->show_all_obb) && obj->obb_VBO != 0)
		{
			c = (is_drawn) ? App->scene_base->obb_color : Grey;

			glColor3ub(c.r * 255.0f, c.g * 255.0f, c.b * 255.0f);
			glLineWidth(App->scene_base->obb_width);

			glBindBuffer(GL_ARRAY_BUFFER, obj->obb_VBO);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabb_IBO);
			glDrawElements(GL_LINES, sizeof(aabb_indices), GL_UNSIGNED_INT, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glColor3ub(255, 255, 255);
		}

		glDisableClientState(GL_VERTEX_ARRAY);

		
	}
	return true;
}

GameObject* ModuleScene::FindById(uint id)
{
	for (uint i = 0; i < gameObjects.size(); i++)
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

