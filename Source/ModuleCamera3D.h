#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start(Config* config = nullptr);
	bool Update(float dt);
	void RotateWithMouse();
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

private:
	void CalculateViewMatrix();

public:
	vec3 X = {1.0f, 0.0f, 0.0f};
	vec3 Y = {0.0f, 1.0f, 0.0f};
	vec3 Z = {0.0f, 0.0f, 1.0f};

	const vec3 c_X = { 1.0f, 0.0f, 0.0f };
	const vec3 c_Y = { 0.0f, 1.0f, 0.0f };
	const vec3 c_Z = { 0.0f, 0.0f, 1.0f };
	vec3 Position, Reference;
	bool viewport_focus;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
};