#pragma once
#include "Panel.h"
#include "glew/include/GL/glew.h"

class Viewport : public Panel
{
public:

	//Panel 
	static const uint default_width = 600;
	static const uint default_height = 600;
	static const uint default_pos_x = 300;
	static const uint default_pos_y = 250;

	Viewport();
	~Viewport();
	void Draw();

	GLuint* texture;
};

