#include "ModuleViewport.h"
#include "Application.h"
#include "Viewport.h"
#include "ModuleEditor.h"


ModuleViewport::ModuleViewport(bool start_enabled) : Module("ModuleViewport", start_enabled)
{
}


ModuleViewport::~ModuleViewport()
{
}


void ModuleViewport::DrawScene()
{
	GenerateBuffers();

	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	/*glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(10.f, 0.f, 0.f);
	glVertex3f(0.f, 10.f, 0.f);
	glEnd();
	glLineWidth(2.0f);*/

	App->editor->tab_viewport->texture = frame_buffer;
	App->editor->tab_viewport->Draw();
}

void ModuleViewport::GenerateBuffers() 
{
	
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	// The texture we're going to render to
	
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Something went wrong creating the frame buffer", 'e');

	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glViewport(0, 0, 600, 600); // Render on the whole framebuffer, complete from the lower left corner to the upper right

	
}