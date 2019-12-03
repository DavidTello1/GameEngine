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

	current_camera = viewport_camera;
}


Viewport::~Viewport()
{
}

// Is not automatically called
bool Viewport::PreUpdate()
{
	if (/*width != window_avail_size.x || height != window_avail_size.y*/true)
	{
		width = window_avail_size.x;
		height = window_avail_size.y;

		frame_buffer.GenerateFBO(width, height);
		OnResize(width, height);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.id);
	glClearColor(current_camera->background.r, current_camera->background.g, current_camera->background.b, current_camera->background.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glEnable(GL_DEPTH_TEST);

	OnCameraUpdate();

	/*if (current_camera->update_projection)
	{
		OnCameraUpdate();
		current_camera->update_projection = false;
	}*/

	return true;
}


void Viewport::Draw() 
{
	PreUpdate();
	window_avail_size = ImGui::GetContentRegionAvail();
	
	ImGui::Image((ImTextureID)frame_buffer.tex, ImVec2(width, height),ImVec2(0,1),ImVec2(1,0));

	//current_camera->DrawFrustum();

	//if (current_camera == viewport_camera)
	//{
		App->scene_base->Draw();
		App->scene->Draw();
	//}

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
	glViewport(0, 0, width, height);

	current_camera->SetAspectRatio(width/height);
}

void Viewport::OnCameraUpdate()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(current_camera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(current_camera->GetViewMatrix());

}