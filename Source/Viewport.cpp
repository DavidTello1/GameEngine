#include "Application.h"
#include "Viewport.h"
#include "ComponentCamera.h"
#include "mmgr/mmgr.h"

const int Viewport::default_width = 600;
const int Viewport::default_height = 600;
const int Viewport::default_pos_x = 300;
const int Viewport::default_pos_y = 250;

Viewport::Viewport() : Panel("Viewport")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	has_menubar = false;
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

		GenerateFBO(ImVec2(width, height));
		OnResize(width, height);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.id);
	glClearColor(viewport_camera->background.r,viewport_camera->background.g,viewport_camera->background.b,viewport_camera->background.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glEnable(GL_DEPTH_TEST);

	if (viewport_camera->update_projection)
	{
		OnCameraUpdate();
		viewport_camera->update_projection = false;
	}

	App->scene_base->Draw();
	App->scene->Draw();

	return true;
}


void Viewport::Draw() 
{
	window_avail_size = ImGui::GetContentRegionAvail();
	
	ImGui::Image((ImTextureID)frame_buffer.tex, ImVec2(width, height),ImVec2(0,1),ImVec2(1,0));
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
	RemoveBuffer(frame_buffer);

	return true;
}

bool Viewport::GenerateFBO(ImVec2 size)
{
	RemoveBuffer(frame_buffer);

	//Generate the FBO and bind it, continue if FBO is complete
	glGenFramebuffers(1, &frame_buffer.id);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.id);

	glGenTextures(1, &frame_buffer.tex);
	glBindTexture(GL_TEXTURE_2D, frame_buffer.tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer.tex, 0);

	//Generate RenderBufferObject
	glGenRenderbuffers(1, &frame_buffer.depth);
	glBindRenderbuffer(GL_RENDERBUFFER, frame_buffer.depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frame_buffer.depth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//OnResize(pos_x, pos_y, width, height);
	return true;
}

void Viewport::RemoveBuffer(FrameBuffer& buffer)
{
	if (buffer.id != 0)
	{
		glDeleteFramebuffers(1, &buffer.id);
		buffer.id = 0;
	}

	if (buffer.depth != 0)
	{
		glDeleteRenderbuffers(1, &buffer.depth);
		buffer.depth = 0;
	}

	if (buffer.tex != 0)
	{
		glDeleteTextures(1, &buffer.tex);
		buffer.tex = 0;
	}
}

void Viewport::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	viewport_camera->SetAspectRatio(width/height);
}

void Viewport::OnCameraUpdate()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewport_camera->GetViewMatrix());

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(viewport_camera->GetProjectionMatrix());
}