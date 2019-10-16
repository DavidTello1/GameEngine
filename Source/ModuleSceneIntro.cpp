#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "glew/include/GL/glew.h"
#include "par_shapes.h"
#include "Viewport.h"
#include "ModuleResources.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module("SceneIntro", start_enabled)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start(Config* config)
{
	LOG("Loading Intro assets", 'v');
	bool ret = true;

	App->camera->Move(vec3(0, 0, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
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

	return true;
}

bool ModuleSceneIntro::Draw()
{
	//glLoadIdentity();

	if (App->editor->show_plane)
		DrawGridPlane();
	
	if (App->editor->show_axis)
		DrawAxis();

	DrawCube();

	App->resources->Draw();

	/*par_shapes_mesh* s = par_shapes_create_cone(10, 10);
	par_shapes_translate(s, 0, 0, 0);

	GLuint my_indices = 0;
	glGenBuffers(1, (GLuint*) &(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*s->ntriangles, s->triangles, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, s->ntriangles, GL_UNSIGNED_INT, NULL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->editor->tab_viewport->frame_buffer.id);
	////glBindVertexArray(s->points);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*s->npoints, s->points, GL_STATIC_DRAW);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	////glDrawArrays(GL_TRIANGLES, 0, s->npoints);
	//glDrawElements(GL_TRIANGLES, s->npoints, GL_UNSIGNED_INT, NULL);
	//glDisableClientState(GL_VERTEX_ARRAY);

	//par_shapes_free_mesh(s);
	*/
	return true;
}

void ModuleSceneIntro::DrawCube()
{
	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	 // Top face (y = 1.0f)
	 // Define vertices in counter-clockwise (CCW) order with normal pointing out
	glColor3ub(255, 0, 255);     // Green
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom face (y = -1.0f)
	glColor3ub(0, 255, 255);     // Orange
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Front face  (z = 1.0f)
	glColor3ub(255, 255, 0);    // Red
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back face (z = -1.0f)
	glColor3ub(0, 0, 255);    // Yellow
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left face (x = -1.0f)
	glColor3ub(0, 255, 0);     // Blue
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right face (x = 1.0f)
	glColor3ub(255, 0, 0);     // Magenta
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();  // End of drawing color-cube
}

void ModuleSceneIntro::DrawGridPlane()
{
	//PLANE -----------------------------
	glLineWidth(1.0f);

	glBegin(GL_LINES);
	glColor3ub(255, 255, 255);
	for (float i = -20; i <= 20; ++i)
	{
		glVertex3f(i, 0.f, 0.f);
		glVertex3f(i, 0, 20.f);

		glVertex3f(0.f, 0.f, i);
		glVertex3f(20.f, 0, i);

		glVertex3f(i, 0.f, 0.f);
		glVertex3f(i, 0, -20.f);

		glVertex3f(0.f, 0.f, i);
		glVertex3f(-20.f, 0, i);
	}
	glEnd();
}

void ModuleSceneIntro::DrawAxis()
{

}