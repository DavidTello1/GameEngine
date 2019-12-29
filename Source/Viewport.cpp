#include "Application.h"
#include "Viewport.h"
#include "ModuleScene.h"
#include "ModuleSceneBase.h"
#include "ComponentCamera.h"
#include "ImGuizmo.h"
#include <string>

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
	if (our_font.textures.empty())
		our_font.init("Assets/Fonts/Dukas.ttf", 45 /* size */);

	ImGuizmo::SetDrawlist();


	if (width != window_avail_size.x || height != window_avail_size.y)
	{
		width = window_avail_size.x;
		height = window_avail_size.y;

		frame_buffer.GenerateFBO(width, height);
		OnResize(width, height);

		ImGuizmo::SetRect(pos_x, pos_y, width, height);
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

	float3 pos = camera->frustum.pos;
	float3 up = camera->frustum.up;
	float3 front = camera->frustum.front;

	camera->SetPosition({ 0,0,1});
	camera->Look({ 0, 0, 0 });

	/////////////////////////////////////
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrtho(Configuration::viewport[Configuration::l], Configuration::viewport[Configuration::r], Configuration::viewport[Configuration::b], -Configuration::viewport[Configuration::t], Configuration::n, Configuration::f);
	glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], 1, -1);

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/////////////////////////////////////
	App->gui->Draw(camera);

	//glColor3f(1, 0, 0);

	//glfreetype::print(camera, our_font, 0 /* xpos */, 80 /* ypos */,
	//	"This text is at (0,80) abcdefghijklmnopqrstuwvxyz");

	//glColor3f(1, 1, 1);

	//glfreetype::print(camera, our_font, 0 /* xpos */, 0 /* ypos */,
	//	"This text is at (0,0) abcdefghijklmnopqrstuwvxyz");

	glPopAttrib();

	glPopMatrix();

	camera->frustum.up = up;
	camera->frustum.front = front;
	camera->SetPosition(pos);

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
