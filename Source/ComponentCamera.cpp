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
	frustum.farPlaneDistance = 100.0f;
	SetFov(60.0f);

	SetAspectRatio(1.4f);

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
		update_projection = true;
	}

}

void ComponentCamera::SetFarPlane(float distance)
{
	if (distance > 0 && distance > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = distance;
		update_projection = true;
	}
}

void ComponentCamera::SetFov(float fov, bool in_degree)
{
	float aspect_ratio = frustum.AspectRatio();

	if (in_degree)
		frustum.verticalFov = DEGTORAD * fov;
	else
		frustum.verticalFov =  fov;

	SetAspectRatio(aspect_ratio);
}

void ComponentCamera::SetAspectRatio(float ratio)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov / 2) * ratio);
	update_projection = true;
}

// Actions ---------------------------------------------------
void ComponentCamera::SetPosition(const float3& position)
{
	frustum.pos = position;
	update_projection = true;
}

void ComponentCamera::Move(const float3 & distance)
{
	frustum.Translate(distance);
	update_projection = true;
}

void ComponentCamera::Look(const float3 & position)
{
	float3 vector = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, vector.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();

	update_projection = true;
}

// Debug -----------------------------------------------------

void ComponentCamera::DrawFrustum() {

	float *proj = frustum.ProjectionMatrix().ptr();
	
	// Get near and far from the Projection matrix.
	const double n = proj[11] / (proj[10] - 1.0);
	const double f = proj[11] / (1.0 + proj[10]);

	// Get the sides of the n plane.
	const double nLeft = n * (proj[2] - 1.0) / proj[0];
	const double nRight = n * (1.0 + proj[2]) / proj[0];
	const double nTop = n * (1.0 + proj[6]) / proj[5];
	const double nBottom = n * (proj[6] - 1.0) / proj[5];

	// Get the sides of the f plane.
	const double fLeft = f * (proj[2] - 1.0) / proj[0];
	const double fRight = f * (1.0 + proj[2]) / proj[0];
	const double fTop = f * (1.0 + proj[6]) / proj[5];
	const double fBottom = f * (proj[6] - 1.0) / proj[5];

	glLineWidth(2);
	glColor3ub(255.0f,0.0f,0.0f);
	glBegin(GL_LINES);
	
	glVertex3f(nLeft, nBottom, -n);
	glVertex3f(fLeft, fBottom, -f);

	glVertex3f(nRight, nBottom, -n);
	glVertex3f(fRight, fBottom, -f);

	glVertex3f(nRight, nTop, -n);
	glVertex3f(fRight, fTop, -f);

	glVertex3f(nLeft, nTop, -n);
	glVertex3f(fLeft, fTop, -f);

	//f
	glVertex3f(fLeft, fBottom, -f);
	glVertex3f(fRight, fBottom, -f);

	glVertex3f(fRight, fTop, -f);
	glVertex3f(fLeft, fTop, -f);

	glVertex3f(fRight, fTop, -f);
	glVertex3f(fRight, fBottom, -f);

	glVertex3f(fLeft, fTop, -f);
	glVertex3f(fLeft, fBottom, -f);

	//n
	glVertex3f(nLeft, nBottom, -n);
	glVertex3f(nRight, nBottom, -n);

	glVertex3f(nRight, nTop, -n);
	glVertex3f(nLeft, nTop, -n);

	glVertex3f(nLeft, nTop, -n);
	glVertex3f(nLeft, nBottom, -n);

	glVertex3f(nRight, nTop, -n);
	glVertex3f(nRight, nBottom, -n);

	glEnd();
	glLineWidth(1);
	glPopMatrix();

	glColor3ub(255.0f, 255.0f, 255.0f);
}