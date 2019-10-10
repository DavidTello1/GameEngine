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
	static const int default_width;
	static const int default_height;
	static const int default_pos_x;
	static const int default_pos_y;

	Viewport();
	~Viewport();

	bool Generate(ImVec2 size = { 600,600 });

	void RemoveBuffer(FrameBuffer & buffer);

	void OnResize(int x, int y, int width, int height);

	bool PreUpdate();

	bool PostUpdate();

	bool CleanUp();

	void Draw();

	FrameBuffer frame_buffer;
};

