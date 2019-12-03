#pragma once
#include "Panel.h"
#include "Imgui/imgui.h"
#include "FrameBuffer.h"

class ComponentCamera;

class Viewport : public Panel
{
public:

	//Panel 
	static const int default_width;
	static const int default_height;
	static const int default_pos_x;
	static const int default_pos_y;

	Viewport(const char* name);
	~Viewport();


	void OnResize(float width, float height);

	//void OnCameraUpdate();

	bool PreUpdate();

	bool PostUpdate();

	bool CleanUp();

	void Draw();

	FrameBuffer frame_buffer;

	ComponentCamera* camera;

	ImVec2 window_avail_size = { default_width, default_height };

};

