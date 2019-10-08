#pragma once

#include "Module.h"
#include "glew/include/GL/glew.h"

struct Framebuffer
	{
		unsigned id = 0;
		unsigned depth = 0;
		unsigned tex = 0;
	};

class ModuleViewport :	public Module
{

public:


	ModuleViewport(bool start_enabled = true);

	~ModuleViewport();
	void DrawScene();
	void RemoveFrameBuffer(Framebuffer & buffer);
	void GenerateBuffer(Framebuffer & buffer);

	

	Framebuffer fbuffer;
};

