#include "ModuleScene.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

GameObject* ModuleScene::root_object;
GameObject* ModuleScene::main_camera;

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

	// Create game objects after this ----------

	main_camera = CreateGameObject("Camera");
	main_camera->AddComponent(Component::Type::Camera);
		
	GameObject* bparent = CreateGameObject("BakerHouse");

	App->resources->LoadResource("Assets/BakerHouse.fbx", Component::Type::Mesh, true, bparent);
	App->resources->LoadResource("Assets/Baker_house.png", Component::Type::Material, true);

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
	UpdateMainCamera(dt);

	return true;
}

void ModuleScene::UpdateMainCamera(float dt)
{
	if (main_camera == nullptr) return;

	ComponentCamera* camera = (ComponentCamera*)main_camera->GetComponent(Component::Type::Camera);

	if (camera == nullptr || camera->viewport_focus == false) return;

	// TODO Change library glmath to math

	// Camera ZOOM with MOUSE WHEEL
	ImGuiIO io = ImGui::GetIO();
	vec3 newPos(0, 0, 0);
	float speed = io.MouseWheel * zoom_speed * dt; // TODO Not hardcoded speed
	if (speed != 0)
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
			speed *= 2.0f;

		newPos -= camera->Z * speed;
		camera->Position += newPos;
	}

	// Center camera to selected gameobject with F
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		LOG("Centering camera from [%f,%f,%f]", camera->Position.x, camera->Position.y, camera->Position.z, 'v');
		// To change to the Reference we want to orbit at
		GameObject* object = GetSelectedGameObject();
		if (object != nullptr)
		{
			float3 c = { camera->Position.x,camera->Position.y,camera->Position.z };
			float3 p = object->bounding_box[9] - c;

			float l = length(vec3(p.x, p.y, p.z));
			float min = l;
			int face = 9;

			for (int i = 10; i < 13; i++)
			{
				p = object->bounding_box[i] - c;
				l = length(vec3(p.x, p.y, p.z));
				if (l < min) {
					min = l;
					face = i;
				}
			}

			vec3 new_p = { object->bounding_box[face].x,object->bounding_box[face].y,object->bounding_box[face].z };
			float size = object->size.MaxElement();
			float offset = Sqrt((size*size) - (size*size / 4));
			float parent = (object->HasChilds()) ? 1.0f : -1.0f;
			
			switch (face)
			{
			case 9:
				camera->Position = new_p + (camera->c_Z * offset * parent);
				break;
			case 10:
				camera->Position = new_p + (camera->c_X * offset* parent);
				break;
			case 11:
				camera->Position = new_p - (camera->c_X * offset* parent);
				break;
			case 12:
				camera->Position = new_p - (camera->c_Z * offset* parent);
				break;
			default:
				LOG("Could not detect closest face", 'e');
				break;
			}
			//mesh->bounding_box[13] = { Position.x, Position.y, Position.z };
			//App->resources->bbox_indices[25] = face;
			//App->resources->GenBoundingBox(mesh);

			LOG("FACE %i", face, 'v');
			LOG("To [%f,%f,%f]", camera->Position.x, camera->Position.y, camera->Position.z, 'v');
			LOG("Looking at [%f,%f,%f]", new_p.x, new_p.y, new_p.z, 'v');

			camera->LookAt(new_p);


		}
	}
	//Free move 
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		//Camera motion with WASD, Shift to go faster
		vec3 newPos(0, 0, 0);
		float speed = 10.0f * dt;

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
			App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
			speed *= 2.0f;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= camera->Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += camera->Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camera->X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camera->X * speed;

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) newPos -= camera->Y * speed;
		else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) newPos += camera->Y * speed;

		camera->Position += newPos;
		camera->Reference += newPos;

		camera->RotateWithMouse();
	}

	// Orbit move
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		camera->RotateWithMouse();
		// To change to the Reference we want to orbit at
		GameObject* object = GetSelectedGameObject();
		if (object != nullptr)
		{
			camera->LookAt({ object->center.x, object->center.y,object->center.z });
		}
	}

	// Recalculate matrix -------------
	camera->CalculateViewMatrix();

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
			if (renderer->show_wireframe || show_all_wireframe) //wireframe
			{
				glColor3ub(wireframe_color[0]*255.0f, wireframe_color[1] * 255.0f, wireframe_color[2] * 255.0f);
				glLineWidth(wireframe_width);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			renderer->Draw();
		}

		// Bounding boxes
		GameObject* obj = gameObjects[i];
		glEnableClientState(GL_VERTEX_ARRAY);
		if ((obj->show_bounding_box || App->scene->show_all_bounding_box) && obj->bb_VBO != 0)
		{
			glColor3ub(App->scene->bounding_box_color[0] * 255.0f, App->scene->bounding_box_color[1] * 255.0f, App->scene->bounding_box_color[2] * 255.0f);
			glLineWidth(App->scene->bounding_box_width);

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

