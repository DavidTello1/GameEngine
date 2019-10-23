#include "Application.h"
#include "Viewport.h"
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
	//GREEN
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	App->scene_intro->Draw();

	//DrawFigures();
	//glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	return true;
}


void Viewport::Draw() 
{
	if (ImGui::IsWindowFocused() == true) // only accept camera controls if viewport is selected
		App->camera->viewport_focus = true;
	else
		App->camera->viewport_focus = false;


	window_avail_size = ImGui::GetContentRegionAvail();
	
	ImGui::Image((ImTextureID)frame_buffer.tex, ImVec2(width, height),ImVec2(0,1),ImVec2(1,0));
}

// Is not automatically called
bool Viewport::PostUpdate()
{
	// second step
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	// BLUE
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
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	projection_matrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&projection_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}