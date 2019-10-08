#include "ModuleViewport.h"
#include "Application.h"
#include "Viewport.h"
#include "ModuleEditor.h"
#include "Imgui/imgui.h"


ModuleViewport::ModuleViewport(bool start_enabled) : Module("ModuleViewport", start_enabled)
{
}


ModuleViewport::~ModuleViewport()
{
}


void ModuleViewport::DrawScene()
{

	
	GenerateBuffer(fbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, fbuffer.id);
	glViewport(0, 0, 600, 600);
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	//glLineWidth(2.0f);
	//glBegin(GL_LINES);
	//glVertex3f(0.f, 0.f, 0.f);
	//glVertex3f(10.f, 0.f, 0.f);
	//glVertex3f(0.f, 10.f, 0.f);
	//glEnd();
	//glLineWidth(2.0f);

	App->editor->tab_viewport->texture = &fbuffer.tex;
	App->editor->tab_viewport->Draw();

	/*ImGui::GetWindowDrawList()->AddImage(
		(void*)fbuffer.tex,
		ImVec2(500, 500),
		ImVec2(500 + 600, 500 + 600));*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Blue background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//screenShader.use();
	//glBindVertexArray(quadVAO);
	//glDisable(GL_DEPTH_TEST);
	//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	
}
void ModuleViewport::RemoveFrameBuffer(Framebuffer& buffer)
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

void ModuleViewport::GenerateBuffer(Framebuffer& buffer)
{
	RemoveFrameBuffer(buffer);
	
	glGenFramebuffers(1, &buffer.id);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer.id);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glGenTextures(1, &buffer.tex);
	glBindTexture(GL_TEXTURE_2D, buffer.tex);

	// Give an empty image to OpenGL ( the last "0" )
	// HARDCODED width and height
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.tex, 0);
	
	//Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	// The depth buffer
	
	glGenRenderbuffers(1, &buffer.depth);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);

	//create and initialize a renderbuffer object's data store
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 600, 600);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer.tex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depth);

	// Unbind render buffer (depth)
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Draw what is attached to attachment0
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Unbind frame buffer (idk)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glGetUniformLocation(buffer.tex, "renderedTexture");
	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Something went wrong creating the frame buffer", 'e');

	//glViewport(0, 0, 600, 600); // Render on the whole framebuffer, complete from the lower left corner to the upper right

	
}