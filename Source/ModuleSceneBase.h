#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "Color.h"

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

	void CameraZoom(float dt);
	void CameraFreeMove(float dt);
	void CameraFocusTo();
	void CameraOrbit(float dt);
	void CameraRotateWithMouse(float dt);

	void CameraMousePicking();

public:

	bool camera_culling = true;

	Ray print_ray;

	float zoom_speed = 75.0f;

	bool Draw();

public:

	float plane_length = 40.0f;
	float axis_length = 40.0f;

	Color wireframe_color = White;
	float wireframe_width = 1.0f;
	bool show_all_wireframe = false;
	
	Color aabb_color = Red;
	float aabb_width = 1.0f;

	Color obb_color = Green;
	float obb_width = 1.0f;

	bool show_all_aabb = false;
	bool show_all_obb = false;

private:

	void DrawGridPlane();
	void DrawAxis();

	
};

// Viewport camera
extern ComponentCamera* viewport_camera;