#include "Application.h"
#include "ModuleSceneBase.h"

#include "glew/include/GL/glew.h"
#include "mmgr/mmgr.h"


ModuleSceneBase::ModuleSceneBase(bool start_enabled) : Module("SceneIntro", start_enabled)
{}

ModuleSceneBase::~ModuleSceneBase()
{}

// Load assets
bool ModuleSceneBase::Start(Config* config)
{
	App->camera->Move(vec3(0, 7.5f, 7.5f));
	App->camera->LookAt(vec3(0, 0, 0));

	return true;
}

// Update
bool ModuleSceneBase::Update(float dt)
{
	return true;
}

bool ModuleSceneBase::PostUpdate(float dt)
{
	return true;
}

bool ModuleSceneBase::CleanUp()
{
	LOG("Unloading Intro scene");
	return true;
}

bool ModuleSceneBase::Draw()
{
	if (App->editor->show_plane) // plane
		DrawGridPlane();

	if (App->editor->show_axis) // axis
		DrawAxis();

	return true;
}

void ModuleSceneBase::DrawGridPlane()
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

void ModuleSceneBase::DrawAxis()
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