#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module("SceneIntro", start_enabled)
{

}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets", 'v');
	bool ret = true;

	App->camera->Move(vec3(0.0f, 10.0f, -20.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
bool ModuleSceneIntro::Update(float dt)
{

	return true;
}

