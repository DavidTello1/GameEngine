#pragma once

#include "Component.h"
#include "glew\include\GL\glew.h"

#include "Globals.h"
#include "glmath.h"
#include "Math.h"

class ComponentCamera : public Component
{
public:

	ALIGN_CLASS_TO_16

	ComponentCamera(GameObject* gameObject);
	~ComponentCamera();

	float* GetViewMatrix();
	float* GetProjectionMatrix();

	void CalculateViewMatrix();
	void CalculateProjectionMatrix();

	float4x4 SetFrustum(float fovY, float aspectRatio, float front, float back);
	float4x4 SetFrustum(float l, float r, float b, float t, float n, float f);
	float4x4 SetOrthoFrustum(float l, float r, float b, float t, float n, float f);

	void DrawFrustum();

public:

	float3 X = float3::unitX;
	float3 Y = float3::unitY;
	float3 Z = float3::unitZ;

	const float3 c_X = float3::unitX;
	const float3 c_Y = float3::unitY;
	const float3 c_Z = float3::unitZ;

	float3 Position, Reference;
	bool viewport_focus;

public:

	bool perspective = true;
	bool update_projection = false;

	float z_near = 1.0f;
	float z_far = 100.0f;

	float fov_y = 60.0f;

	float width = 1280.0f;
	float height = 720.0f;

public:

	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;

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
