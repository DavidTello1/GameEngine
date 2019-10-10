#pragma once
#include "Panel.h"
#include "glew/include/GL/glew.h"
#include "Imgui/imgui.h"

struct FrameBuffer
{
	unsigned int id;
	unsigned int tex;
	unsigned int depth;
};

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

	bool Generate(ImVec2 size = { 600,600 });

	bool PreUpdate();

	bool PostUpdate();

	bool CleanUp();

	void Draw();

	FrameBuffer frame_buffer;
};

