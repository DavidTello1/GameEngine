#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleResources.h"
#include "Hierarchy.h"
#include "Viewport.h"
#include "GameObject.h"

#include "glew/include/GL/glew.h"
#include "par_shapes.h"
#include "mmgr/mmgr.h"


ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module("SceneIntro", start_enabled)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start(Config* config)
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
		CreateShape((shape_type)i, 9, 9, i * 7.5 -50, 2.5f, -10);
	}

	return ret;
}

void ModuleSceneIntro::CreateShape(shape_type type, int slices, int stacks, float x, float y, float z)
{
	// new GameObject
	CreateGameObj(Mesh::shape_to_string[type]);

	Mesh* m = new Mesh();
	m->CreateMesh(type, slices, stacks,x,y,z);

	App->resources->meshes.push_back(m);
	selected_gameobj->meshes.push_back(m);

}

// Update
bool ModuleSceneIntro::Update(float dt)
{
	return true;
}

bool ModuleSceneIntro::PostUpdate(float dt)
{

	return true;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	for (uint i = 0; i < gameobjs.size(); ++i)
	{
		delete gameobjs[i];
	}
	gameobjs.clear();

	return true;
}

bool ModuleSceneIntro::Draw()
{
	if (App->editor->show_plane) // plane
		DrawGridPlane();
	
	if (App->editor->show_axis) // axis
		DrawAxis();

	if (App->editor->show_wireframe) //wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	App->resources->Draw();

	return true;
}

GameObject* ModuleSceneIntro::CreateGameObj(const char* name)
{
	create_gameobj = true;

	GameObject* obj = new GameObject(name);
	gameobjs.push_back(obj);

	App->editor->tab_hierarchy->AddNode(obj); // add node to hierarchy
	App->scene_intro->selected_gameobj = obj; // new obj is selected_gameobj

	return obj;
}

void ModuleSceneIntro::DeleteGameobj(GameObject* obj)
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

void ModuleSceneIntro::DrawGridPlane()
{
	glLineWidth(0.25f);

	glBegin(GL_LINES);
	glColor3ub(100, 100, 100);
	float d = 40;
	for (float i = -d; i <= d; ++i)
	{
		glVertex3f(i, 0.f, 0.f);
		glVertex3f(i, 0, d);

		glVertex3f(0.f, 0.f, i);
		glVertex3f(d, 0, i);

		glVertex3f(i, 0.f, 0.f);
		glVertex3f(i, 0, -d);

		glVertex3f(0.f, 0.f, i);
		glVertex3f(-d, 0, i);
	}
	glEnd();
}

void ModuleSceneIntro::DrawAxis()
{
	glLineWidth(3.0f);
	glBegin(GL_LINES);

	// x
	glColor3ub(255, 0, 0);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	//y
	glColor3ub(0, 255, 0);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	// z
	glColor3ub(0, 0, 255);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glColor3ub(255, 255, 255);


	glEnd();
}