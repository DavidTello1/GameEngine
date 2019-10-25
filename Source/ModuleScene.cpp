#include "ModuleScene.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ComponentRenderer.h"


ModuleScene::ModuleScene(bool start_enabled) : Module("Scene", start_enabled)
{
}


ModuleScene::~ModuleScene()
{
}

// Load assets
bool ModuleScene::Start(Config* config)
{
	LOG("Loading Intro assets", 'v');
	bool ret = true;

	App->camera->Move(vec3(0, 7.5f, 7.5f));
	App->camera->LookAt(vec3(0, 0, 0));

	//m = App->resources->LoadFBX("Assets/Warrior.fbx");
	//m = GeometryLoader::LoadModel("Assets/Warrior.fbx");
	//App->resources->LoadResource("Assets/warrior.fbx");

	// Loading bakerhouse into the empty
	//GameObject* baker_parent = CreateGameObj("BakerHouse");
	
	App->resources->LoadResource("Assets/BakerHouse.fbx",Component::Type::Mesh,true);
	//App->resources->LoadResource("Assets/Baker_house.png", Component::Type::Material, true);

	//CreateGameObj("House", baker_parent->GetUID());
	//selected_gameobj->SetMesh(App->resources->meshes.at(1));
	//selected_gameobj->GetMesh()->TEX = App->resources->textures.back();

	//CreateGameObj("Chimney", baker_parent->GetUID());
	//selected_gameobj->SetMesh(App->resources->meshes.at(0));
	//selected_gameobj->GetMesh()->TEX = App->resources->textures.back();


	GameObject* pparent = CreateGameObj("ParShapes");
	for (int i = 0; i < shape_type::UNKNOWN; i++)
	{
		App->resources->CreateShape((shape_type)i, 9, 9, i * 7.5 - 50, 2.5f, -10,0.5f, pparent->GetUID());
	}

	return ret;
}

void ModuleScene::CreateShape(shape_type type, int slices, int stacks, float x, float y, float z,uint parent_id)
{
	/*CreateGameObj(Mesh::shape_to_string[type],parent_id);

	Mesh* m = new Mesh();
	m->CreateMesh(type, slices, stacks, x, y, z);

	App->resources->meshes.push_back(m);
	selected_gameobj->SetMesh(m);*/

}

bool ModuleScene::Update(float dt)
{
	return true;
}

bool ModuleScene::PostUpdate(float dt)
{
	return true;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading Main scene");

	for (uint i = 0; i < gameobjs.size(); ++i)
	{
		delete gameobjs[i];
	}
	gameobjs.clear();

	return true;
}

bool ModuleScene::Draw()
{

	if (App->editor->show_wireframe) //wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw GameObjects
	for (uint i = 0; i < gameobjs.size(); ++i)
	{
		glPushMatrix();
		glMultMatrixf(gameobjs[i]->GetLocalTransform().ptr());

		ComponentRenderer* Renderer = (ComponentRenderer*)gameobjs[i]->GetComponent(Component::Type::Renderer);
		if (Renderer != nullptr && gameobjs[i]->GetComponent(Component::Type::Renderer)->IsActive())
			Renderer->Draw();

		glPopMatrix();
	}
	return true;
}

GameObject* ModuleScene::CreateGameObj(const char* name, const uint parent_id, bool visible)
{
	create_gameobj = true;

	GameObject* obj = new GameObject(name);
	if (visible)
		obj->AddComponent(Component::Type::Renderer);

	gameobjs.push_back(obj);


	App->editor->tab_hierarchy->AddNode(obj, App->editor->tab_hierarchy->SearchById(parent_id)); // add node to hierarchy
	selected_gameobj = obj; // new obj is selected_gameobj

	return obj;
}

void ModuleScene::DeleteGameobj(GameObject* obj)
{
	if (selected_gameobj == obj)
		selected_gameobj = nullptr;

	for (int i = 0; i < gameobjs.size(); i++)
	{
		if (gameobjs[i] == obj)
		{
			delete gameobjs[i];
			gameobjs.erase(gameobjs.begin() + i);
			break;
		}
	}
}

void ModuleScene::EraseFromSelected(GameObject * go)
{
	if (go == nullptr) return;

	for (uint i = 0; i < selected_go.size(); i++)
	{
		if (selected_go[i] != nullptr && selected_go[i]->GetUID() == go->GetUID())
		{
			selected_go.erase(selected_go.begin() + i);
			return;
		}
	}
}
