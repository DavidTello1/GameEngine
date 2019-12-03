#include "FrameBuffer.h"
#include "glew/include/GL/glew.h"
#include "Globals.h"

#include "mmgr/mmgr.h"

FrameBuffer::FrameBuffer()
{
}


FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::GenerateFBO(int width, int height)
{
	Remove();

	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	//Generate RenderBufferObject
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Remove()
{
	if (id != 0)
	{
		glDeleteFramebuffers(1, &id);
		id = 0;
	}

	if (depth != 0)
	{
		glDeleteRenderbuffers(1, &depth);
		depth = 0;
	}

	if (tex != 0)
	{
		glDeleteTextures(1, &tex);
		tex = 0;
	}
}
