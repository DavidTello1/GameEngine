#include "Application.h"
#include "Viewport.h"
#include "ModuleScene.h"
#include "ModuleSceneBase.h"
#include "ComponentCamera.h"

#include "mmgr/mmgr.h"

const int Viewport::default_width = 600;
const int Viewport::default_height = 600;
const int Viewport::default_pos_x = 300;
const int Viewport::default_pos_y = 250;

Viewport::Viewport(const char* name) : Panel(name)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	camera = viewport_camera;
}


Viewport::~Viewport()
{
}

// Is not automatically called
bool Viewport::PreUpdate()
{
	if (width != window_avail_size.x || height != window_avail_size.y)
	{
		width = window_avail_size.x;
		height = window_avail_size.y;

		frame_buffer.GenerateFBO(width, height);
		OnResize(width, height);
	}

	glViewport(0, 0, width, height);

	// Binding camera buffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.id);
	glClearColor(camera->background.r, camera->background.g, camera->background.b, camera->background.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glEnable(GL_DEPTH_TEST);

	// Loading camera matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadTransposeMatrixf(camera->projection_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadTransposeMatrixf(camera->view_matrix);

	return true;
}


void Viewport::Draw() 
{
	PreUpdate();

	window_avail_size = ImGui::GetContentRegionAvail();
	
	ImGui::Image((ImTextureID)frame_buffer.tex, ImVec2(width, height),ImVec2(0,1),ImVec2(1,0));

	// Scene drawing
	App->scene->Draw();
	if (camera == viewport_camera)
	{
		App->scene_base->Draw();
	}

	PostUpdate();
}

// Is not automatically called
bool Viewport::PostUpdate()
{
	// Background color of the editor (ImGui)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	return true;
}

bool Viewport::CleanUp()
{
	frame_buffer.Remove();

	return true;
}

void Viewport::OnResize(float width, float height)
{
	camera->SetAspectRatio(width/height);
}
