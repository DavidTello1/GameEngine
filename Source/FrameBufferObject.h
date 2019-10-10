#pragma once
#include "Imgui/imgui.h"

class FrameBufferObject
{

public:

	FrameBufferObject();
	~FrameBufferObject();

	bool Generate(ImVec2 size = { 600,600 });

	bool PreUpdate();

	bool PostUpdate();

	bool CleanUp();

public:

	unsigned int id;
	unsigned int tex;
	unsigned int depth;


};

