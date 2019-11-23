#include "ComponentCamera.h"
#include "Globals.h"
#include "Application.h"
#include "Imgui/imgui.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ModuleScene.h"
#include "mmgr/mmgr.h"


ComponentCamera::ComponentCamera(GameObject* gameobj) : Component(Component::Type::Camera, gameobj)
{
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3::zero;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 75.0;
	SetFov(60.0f);

	aspect_ratio = 1.4f;
	SetAspectRatio(aspect_ratio);

	update_projection = true;
}

ComponentCamera::~ComponentCamera()
{}

// Getters -----------------------------------------------------------------

float ComponentCamera::GetNearPlane() const
{
	return frustum.nearPlaneDistance;
}

float ComponentCamera::GetFarPlane() const
{
	return frustum.farPlaneDistance;
}

float ComponentCamera::GetFOV(bool in_degree) const
{
	if (in_degree)
		return frustum.verticalFov * RADTODEG;

	return frustum.verticalFov;
}
float ComponentCamera::GetHorizontalFOV(bool in_degree) const
{
	if (in_degree)
		return frustum.horizontalFov * RADTODEG;

	return frustum.horizontalFov;
}

float ComponentCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

float* ComponentCamera::GetViewMatrix()
{
	math::float4x4 matrix = frustum.ViewMatrix();
	return matrix.Transposed().ptr();
}

float* ComponentCamera::GetProjectionMatrix()
{
	math::float4x4 matrix = frustum.ProjectionMatrix();
	return matrix.Transposed().ptr();
}

// Setters -----------------------------------------------------------------

void ComponentCamera::SetNearPlane(float distance)
{
	if (distance > 0 && distance < frustum.farPlaneDistance)
	{
		frustum.nearPlaneDistance = distance;
		UpdatePlanes();
		update_projection = true;
	}

}

void ComponentCamera::SetFarPlane(float distance)
{
	if (distance > 0 && distance > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = distance;
		UpdatePlanes();
		update_projection = true;
	}
}

void ComponentCamera::SetFov(float fov, bool in_degree)
{
	if (in_degree)
		frustum.verticalFov = DEGTORAD * fov;
	else
		frustum.verticalFov =  fov;
	
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov / 2) * aspect_ratio);

	UpdatePlanes();
	update_projection = true;
}

void ComponentCamera::SetAspectRatio(float ratio)
{
	aspect_ratio = ratio;
	UpdatePlanes();
	SetFov(frustum.verticalFov, false);	
}

// Actions ---------------------------------------------------
void ComponentCamera::SetPosition(const float3& position)
{
	frustum.pos = position;
	UpdatePlanes();
	update_projection = true;
}

void ComponentCamera::Move(const float3 & distance)
{
	frustum.Translate(distance);
	UpdatePlanes();
	update_projection = true;
}

void ComponentCamera::Look(const float3 & position)
{
	float3 vector = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, vector.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
	
	UpdatePlanes();
	update_projection = true;
}

void ComponentCamera::UpdatePlanes()
{
	frustum.GetPlanes(planes);
}

bool ComponentCamera::ContainsAABB(const AABB& refBox) const
{
	float3 corners[8];
	int total_in = 0;
	refBox.GetCornerPoints(corners);
	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (planes[p].IsOnPositiveSide(corners[i])) {
				iPtIn = 0;
				--iInCount;
			}
		}
		if (iInCount == 0)
			return false;
			//return(AABB_OUT);
		// check if they were all on the right side of the plane
		total_in += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (total_in == 6)
		return true;
		//return(AABB_IN);
	// we must be partly in then otherwise
	//return(INTERSECT);
	return true;
}

// Debug -----------------------------------------------------

void ComponentCamera::DrawFrustum()
{
	glBegin(GL_LINES);
	glLineWidth(2.0f);
	glColor4f(Red.r, Red.g, Red.b, Red.a);

	for (uint i = 0; i < frustum.NumEdges(); i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}