#include "ModuleScene.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
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

	test_camera_obj = CreateGameObject("test camera");
	test_camera_obj->AddComponent(Component::Type::Camera);

	test_camera = test_camera_obj->GetComponent<ComponentCamera>();
	
	//ResourceModel* tcmodel = new ResourceModel(root_object->GetUID());
	//std::string tmdp = "MyCamera.dvs";
	//tcmodel->Import("/Assets/camera_mesh.fbx", tmdp);

	//for (int i = 0; i < 6; i++) {

	//ResourceModel* bhmodel = new ResourceModel(root_object->GetUID());
	//std::string tmp = "MyBakerHouse.dvs";
	//bhmodel->Import("/Assets/BakerHouse.fbx",tmp);

	//
	//gameObjects[2+i*3]->SetLocalPosition({ 5.0f*i,0.0f,-5.0f* i });
	//}

	App->resources->ImportFromOutside("D:/Users/William/Desktop/Assets/Tank/tank.fbx");

	App->resources->ImportFromOutside("D:/Users/William/Desktop/Assets/BakerHouse.fbx");

	gameObjects[2]->SetLocalPosition({ 0.0f,0.0f,20.0f });

	quadtree = new Quadtree(AABB({ -20,-20,-20 }, { 20,20,20 }));

	
	//quadtree->AddGameObject(bhmodel.)
	//UnSelectAll();

	return true;
}

bool ModuleScene::Update(float dt)
{
	if (test_camera)
		test_camera->DrawFrustum();

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

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{

		//quadtree->RemoveGameObject(gameObjects.back());
		quadtree->OptimizeSpace();

	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{

		quadtree->RemoveGameObject(gameObjects.back());

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

			if (test_camera_obj != obj)
			{
				quadtree->Clear();
				
				for (GameObject* obj : gameObjects)
				{
					quadtree->AddGameObject(obj);
				}
			}

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

	return true;
}

bool ModuleScene::Draw()
{
	// ALL this need a big rework
	quadtree->ResetCullingState();

	// Cheap fix
	glColor3ub(255, 255, 255);

	Color c;
	std::vector< GameObject*> candidates;
	quadtree->CollectCandidates(candidates, test_camera->frustum);

	for ( GameObject* obj : candidates)
	{

		c = (!obj->HasChilds()) ? App->scene_base->aabb_color : Cyan;
		// Just boxes colors things
		//obj->is_drawn = true;

		// Draw GameObjects
		if (!App->scene_base->camera_culling || test_camera->frustum.DO_Intersects(obj->aabb))
		{
			//test_camera->frustum.Intersects()
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
					glColor3ub(255, 255, 255);
				}
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				
				renderer->Draw();
			}
			glPopMatrix();
		}
		else
		{
			obj->is_drawn = false;
		}
	}

	for (GameObject* obj : gameObjects)
	{		
		glEnableClientState(GL_VERTEX_ARRAY);

		// AABB
		if ((obj->show_aabb || App->scene_base->show_all_aabb) && obj->aabb_VBO != 0)
		{
			c = (obj->is_drawn) ? App->scene_base->aabb_color : LightGrey;

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
			c = (obj->is_drawn) ? App->scene_base->obb_color : DarkGrey;

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

		//obj->is_drawn = true;
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
	is_creating = true;

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

GameObject * ModuleScene::PickFromRay(Ray ray)
{
	quadtree->CollectCandidates(pick_candidates, ray);
	std::map<float, GameObject*>::iterator it = pick_candidates.begin();

	// candidates are in order, first to be picked will be the closest one
	for (;it != pick_candidates.end(); it++)
	{
		ComponentMesh* c_mesh = it->second->GetComponent<ComponentMesh>();
		if (!c_mesh) continue;

		ResourceMesh* mesh = c_mesh->GetMesh();
		if (!mesh) continue;
		
		Ray tmpRay = ray;
		tmpRay.Transform(it->second->GetGlobalTransform().Inverted());

		for (uint i = 0; i < mesh->num_indices; i += 3)
		{
			float3 one	 = mesh->vertices[mesh->indices[i]];
			float3 two	 = mesh->vertices[mesh->indices[i+1]];
			float3 three = mesh->vertices[mesh->indices[i+2]];

			if (tmpRay.Intersects(Triangle(one,two,three), nullptr, nullptr))
			{
				UnSelectAll();
				it->second->Select();
				return it->second;
			}
		}
		
	}

	return nullptr;
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