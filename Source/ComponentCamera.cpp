#include "ComponentCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
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

	Move({ 0,0,1 });
	Look({ 0,0,0 });

	view_matrix4x4 = frustum.ViewMatrix();
	view_matrix = view_matrix4x4.ptr();

	origin_view_matrix = view_matrix;

	projection_matrix4x4 = frustum.ProjectionMatrix();
	projection_matrix = projection_matrix4x4.ptr();

	origin_projection_matrix = projection_matrix;

	//UpdateMatrices();
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::DrawInspector()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Promote to viewport"))
		{
			App->editor->AddViewport(this);
		}
		ImGui::ColorEdit3("Background", (float*)&background);

		// Dummy floats
		float _fov = frustum.verticalFov * RADTODEG;
		float _near = frustum.nearPlaneDistance;
		float _far = frustum.farPlaneDistance;

		if (ImGui::Checkbox("Perspective / Orthogonal", &perspective))
		{
			if (perspective)
				frustum.type = FrustumType::PerspectiveFrustum;
			else
				frustum.type = FrustumType::OrthographicFrustum;

			UpdateMatrices();
		}

		ImGui::Text("Aspect ratio: "); ImGui::SameLine(); ImGui::TextColored({ 1,1,0,1 }, "%f", GetAspectRatio());
		ImGui::Text("Vertical FOV: "); ImGui::SameLine(); ImGui::TextColored({ 1,1,0,1 }, "%f", GetFOV());
		ImGui::Text("Horizontal FOV: "); ImGui::SameLine(); ImGui::TextColored({ 1,1,0,1 }, "%f", GetHorizontalFOV());

		//if (ImGui::DragFloat("Fov Y", &fov, 0.001f, 0.01f, PI)) //radians
		if (ImGui::DragFloat("Fov Y", &_fov, 0.1f, 0.1f, 180.0f, "%.1f"))
		{
			SetFov(_fov, true);
		}
		if (ImGui::DragFloat("Z Near", &_near, 0.25f, 0.1f, _far))
		{
			SetNearPlane(_near);
		}
		if (ImGui::DragFloat("Z Far", &_far, 0.25f, _near, 5000.0f))
		{
			SetFarPlane(_far);
		}
	}
}


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


void ComponentCamera::UpdateMatrices()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}
void ComponentCamera::UpdateViewMatrix()
{
	view_matrix4x4 = frustum.ViewMatrix();
	view_matrix = view_matrix4x4.ptr();
}

void ComponentCamera::UpdateProjectionMatrix()
{
	projection_matrix4x4 = frustum.ProjectionMatrix();
	projection_matrix = projection_matrix4x4.ptr();
}

// Setters -----------------------------------------------------------------

void ComponentCamera::SetNearPlane(float distance)
{
	if (distance > 0 && distance < frustum.farPlaneDistance)
	{
		frustum.nearPlaneDistance = distance;
		UpdateMatrices();
	}

}

void ComponentCamera::SetFarPlane(float distance)
{
	if (distance > 0 && distance > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = distance;
		UpdateMatrices();
	}
}

void ComponentCamera::SetFov(float fov, bool in_degree)
{
	if (in_degree)
		frustum.verticalFov = DEGTORAD * fov;
	else
		frustum.verticalFov =  fov;
	
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov / 2) * aspect_ratio);

	UpdateMatrices();
}

void ComponentCamera::SetAspectRatio(float ratio)
{
	aspect_ratio = ratio;
	SetFov(frustum.verticalFov, false);	
}

// Actions ---------------------------------------------------
void ComponentCamera::SetPosition(const float3& position)
{
	frustum.pos = position;
	UpdateMatrices();
}

void ComponentCamera::Move(const float3 & distance)
{
	frustum.Translate(distance);
	UpdateMatrices();
}

void ComponentCamera::Look(const float3 & position)
{
	float3 vector = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, vector.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
	
	UpdateMatrices();
}

void ComponentCamera::LookNoUpdate(const float3 & position)
{
	float3 vector = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, vector.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
}

// Debug -----------------------------------------------------

void ComponentCamera::DrawFrustum()
{
	glBegin(GL_LINES);
	glLineWidth(1.0f);
	glColor4f(Color::white.r, Color::white.g, Color::white.b, Color::white.a);

	for (int i = 0; i < frustum.NumEdges(); i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}