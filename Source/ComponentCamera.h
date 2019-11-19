#pragma once

#include "Component.h"
#include "glew\include\GL\glew.h"

#include "Globals.h"
#include "glmath.h"
#include "Math.h"

class ComponentCamera : public Component
{
public:

	ComponentCamera(GameObject* gameObject);
	~ComponentCamera();

	float* GetViewMatrix();

	float * GetProjectionMatrix();

	void CalculateViewMatrix();

	void CalculateProjectionMatrix();

	mat4x4 SetFrustum(float l, float r, float b, float t, float n, float f);

	void DrawFrustum();

	mat4x4 SetFrustum(float fovY, float aspectRatio, float front, float back);

	mat4x4 SetOrthoFrustum(float l, float r, float b, float t, float n, float f);

public:
	vec3 X = { 1.0f, 0.0f, 0.0f };
	vec3 Y = { 0.0f, 1.0f, 0.0f };
	vec3 Z = { 0.0f, 0.0f, 1.0f };

	const vec3 c_X = { 1.0f, 0.0f, 0.0f };
	const vec3 c_Y = { 0.0f, 1.0f, 0.0f };
	const vec3 c_Z = { 0.0f, 0.0f, 1.0f };

	vec3 Position, Reference;
	bool viewport_focus;

public:

	bool perspective = true;

	float z_near = 1.0f;
	float z_far = 100.0f;

	float fov_y = 60.0f;

	float width = 1280.0f;
	float height = 720.0f;

	Frustum frustum;
private:

	mat4x4 ViewMatrix;
	mat4x4 ProjectionMatrix;


	float3 frustum_vertices[8];

	GLuint VBO = 0;
	GLuint IBO = 0;

	static GLuint frustum_indices[24];
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
