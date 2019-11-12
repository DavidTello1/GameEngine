#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ModuleSceneBase : public Module
{
public:
	ModuleSceneBase(bool start_enabled = true);
	~ModuleSceneBase();

	bool Start(Config* config = nullptr);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	bool Draw();

	float plane_length = 40.0f;
	float axis_length = 40.0f;

	// Misc
	float wireframe_color[3] = { 1.0f, 1.0f, 1.0f };
	float wireframe_width = 1.0f;
	bool show_all_wireframe = false;

	float bounding_box_color[3] = { 1.0f, 0.0f, 0.0f };
	float bounding_box_width = 1.0f;
	bool show_all_bounding_box = false;

private:

	void DrawGridPlane();
	void DrawAxis();

};

