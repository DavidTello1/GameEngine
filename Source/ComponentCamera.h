#pragma once

#include "Component.h"

#include "Globals.h"
#include "glmath.h"

class ComponentCamera : public Component
{
public:

	ComponentCamera(GameObject* gameObject);
	~ComponentCamera();

	void RotateWithMouse();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

	void CalculateViewMatrix();

public:
	vec3 X = { 1.0f, 0.0f, 0.0f };
	vec3 Y = { 0.0f, 1.0f, 0.0f };
	vec3 Z = { 0.0f, 0.0f, 1.0f };

	const vec3 c_X = { 1.0f, 0.0f, 0.0f };
	const vec3 c_Y = { 0.0f, 1.0f, 0.0f };
	const vec3 c_Z = { 0.0f, 0.0f, 1.0f };

	vec3 Position, Reference;
	bool viewport_focus;

private:

	float z_near;
	float z_far;

	mat4x4 ViewMatrix;
};


// ----------------------------- camera as component draft ------------------------------------------
//#pragma once
//
//#include "Component.h"
//#include "Globals.h"
//#include "Math.h"
//
//class ComponentCamera :	public Component
//{
//
//public:
//
//	ComponentCamera(GameObject* gameObject);
//	~ComponentCamera();
//
//	bool Update(float dt);
//
//	void FocusObject(GameObject* object = nullptr);
//
//	void RotateWithMouse();
//
//	void LookAt(const float3 & Spot);
//
//	void Move(const float3 & Movement);
//
//	const float4x4 * GetViewMatrix(); const
//	const float4x4 * GetViewMatrixInverse(); const
//
//	void CalculateViewMatrix();
//
//
//	float3 reference = { 0,0,0 };
//	float3 position = { 0,0,0 };
//
//
//
//
//private:
//
//	float4x4 view_matrix;
//	
//	const float3 c_X = { 1.0f,0.0f,0.0f };
//	const float3 c_Y = { 0.0f,1.0f,0.0f };
//	const float3 c_Z = { 0.0f,0.0f,1.0f };
//
//	float3x3 M = float3x3::identity;
//};
//
