#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "ComponentCamera.h"

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
	void UpdateMainCamera(float dt);
	void CameraZoom(float dt);
	void CameraFreeMove(float dt);
	void CameraFocusTo();
	void CameraOrbit(float dt);
	void CameraRotateWithMouse(float dt);
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

	// Viewport camera
	static GameObject* main_camera_object;
	static ComponentCamera* main_camera;

	//void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	//void LookAt(const float3 &Spot);
	//void Move(const float3 &Movement);


	float zoom_speed = 75.0f;
private:

	void DrawGridPlane();
	void DrawAxis();

};

