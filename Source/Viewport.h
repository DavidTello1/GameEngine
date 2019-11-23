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

	bool GenerateFBO(ImVec2 size = { static_cast<float>(default_width), static_cast<float>(default_height )});

	void RemoveBuffer(FrameBuffer & buffer);

	void OnResize(float width, float height);

	void OnCameraUpdate();

	bool PreUpdate();

	bool PostUpdate();

	bool CleanUp();

	void Draw();

	FrameBuffer frame_buffer;

	ImVec2 window_avail_size = { default_width, default_height };

};

