#pragma once

#include "Component.h"
#include "glew\include\GL\glew.h"

#include "Globals.h"
#include "glmath.h"
#include "Math.h"
#include "Color.h"

#define AABB_OUT 0
#define AABB_IN	1
#define INTERSECT 2

class ComponentCamera : public Component
{
public:

	ALIGN_CLASS_TO_16

	ComponentCamera(GameObject* gameObject);
	~ComponentCamera();

	static inline Component::Type GetType() { return Component::Type::Camera; }

	//  -------------------------------------- Getters
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV(bool in_degree = true) const;
	float GetHorizontalFOV(bool in_degree = true) const;
	float GetAspectRatio() const;

	float* GetViewMatrix();
	float* GetProjectionMatrix();

	//  -------------------------------------- Setters
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetFov(float fov, bool in_degree = true);
	void SetAspectRatio(float ratio);

	//  -------------------------------------- Actions
	void SetPosition(const float3 & position);
	void Move(const float3 & distance);

	void Look(const float3 & position);

	void UpdatePlanes();

	void DrawFrustum();

	//  -------------------------------------- Consultors
	bool ContainsAABB(const AABB & refBox) const;


	bool viewport_focus = true;
	bool perspective = true;
	bool update_projection = false;

	float aspect_ratio = 0.0f;

	Frustum frustum;
	Plane planes[6];
	Color background = DarkGrey;
};