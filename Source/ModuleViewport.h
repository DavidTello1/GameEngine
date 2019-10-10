#pragma once

#include "Module.h"
#include "glew/include/GL/glew.h"

struct Framebuffer
	{
		unsigned int id = 0;
		unsigned int tex = 0;
		unsigned int depth = 0;
	};

class ModuleViewport :	public Module
{

public:


	ModuleViewport(bool start_enabled = true);

	~ModuleViewport();
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	void DrawScene();

	void RemoveFrameBuffer(Framebuffer & buffer);
	void GenerateBuffer(Framebuffer & buffer);

	bool CleanUp();

	

	Framebuffer fbuffer;
};

