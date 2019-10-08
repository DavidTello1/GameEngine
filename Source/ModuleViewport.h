#pragma once

#include "Module.h"
#include "glew/include/GL/glew.h"


class ModuleViewport :	public Module
{

public:


	ModuleViewport(bool start_enabled = true);

	~ModuleViewport();
	void DrawScene();
	void GenerateBuffers();

	GLuint frame_buffer = 0;
	GLuint renderedTexture;
	GLuint depthrenderbuffer;
};

