#include "ModuleScene.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "Hierarchy.h"


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
	App->resources->LoadResource("Assets/BakerHouse.fbx");
	App->resources->LoadResource("Assets/Baker_house.png");
	for (int i = 0; i < shape_type::UNKNOWN; i++)
	{
		CreateShape((shape_type)i, 9, 9, i * 7.5 - 50, 2.5f, -10);
	}

	return ret;
}

void ModuleScene::CreateShape(shape_type type, int slices, int stacks, float x, float y, float z)
{
	// new GameObject
	CreateGameObj(Mesh::shape_to_string[type]);

	Mesh* m = new Mesh();
	m->CreateMesh(type, slices, stacks, x, y, z);

	App->resources->meshes.push_back(m);
	selected_gameobj->SetMesh(m);

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
	for (const GameObject* obj : gameobjs)
	{

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, obj->mesh->VBO);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindTexture(GL_TEXTURE_2D, obj->mesh->TEX);
		glBindBuffer(GL_ARRAY_BUFFER, obj->mesh->tex_coords_id);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->mesh->IBO);
		glDrawElements(GL_TRIANGLES, obj->mesh->num_indices, GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
		
	}
	return true;
}

GameObject* ModuleScene::CreateGameObj(const char* name)
{
	create_gameobj = true;

	GameObject* obj = new GameObject(name);
	gameobjs.push_back(obj);

	App->editor->tab_hierarchy->AddNode(obj); // add node to hierarchy
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