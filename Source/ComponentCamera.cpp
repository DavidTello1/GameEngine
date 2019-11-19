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
	CalculateViewMatrix();
	CalculateProjectionMatrix();

	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0.0f, 0.0f, -10.0f);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 100.0f;
	SetFov( 60.0f);

	SetAspectRatio(1.4f);

	update_projection = true;

	Position = float3(0.0f, 0.0f, 10.0f);
	Reference = Position;
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
	if (perspective) {

		math::float4x4 matrix = frustum.ViewMatrix();
		return matrix.Transposed().ptr();
	}

	return ViewMatrix.ptr();
}

float* ComponentCamera::GetProjectionMatrix()
{
	if (perspective) {

		math::float4x4 matrix = frustum.ProjectionMatrix();
		return matrix.Transposed().ptr();
	}
	return ProjectionMatrix.ptr();
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

void ComponentCamera::SetPosition(const float3& position)
{
	frustum.pos = position;
	update_projection = true;
}
// Actions ---------------------------------------------------

void ComponentCamera::Look(const float3 & position)
{
	float3 vector = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, vector.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
}

void ComponentCamera::Move(const float3 & distance)
{
	frustum.Translate(distance);
	update_projection = true;
}

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

// To delete -----------------------------------------------

void ComponentCamera::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -Dot(X, Position), -Dot(Y, Position), -Dot(Z, Position), 1.0f);
}

void ComponentCamera::CalculateProjectionMatrix()
{
	ProjectionMatrix = SetFrustum(frustum.verticalFov, frustum.AspectRatio(), frustum.nearPlaneDistance, frustum.farPlaneDistance);
}

float4x4 ComponentCamera::SetFrustum(float l, float r, float b, float t, float n, float f)
{

	float4x4 matrix = float4x4::identity;
	matrix.ptr()[0] = 2 * n / (r - l);
	matrix.ptr()[5] = 2 * n / (t - b);
	matrix.ptr()[8] = (r + l) / (r - l);
	matrix.ptr()[9] = (t + b) / (t - b);
	matrix.ptr()[10] = -(f + n) / (f - n);
	matrix.ptr()[11] = -1;
	matrix.ptr()[14] = -(2 * f * n) / (f - n);
	matrix.ptr()[15] = 0;

	return matrix;
}

float4x4 ComponentCamera::SetFrustum(float fovY, float aspectRatio, float front, float back)
{
	float tangent = tanf(fovY / 2);   // tangent of half fovY
	float height = front * tangent;           // half height of near plane
	float width = height * aspectRatio;       // half width of near plane


	return SetFrustum(-width, width, -height, height, front, back);
}





float4x4 ComponentCamera::SetOrthoFrustum(float l, float r, float b, float t, float n, float f)
{
	float4x4 matrix;
	matrix.ptr()[0] = 2 / (r - l);
	matrix.ptr()[5] = 2 / (t - b);
	matrix.ptr()[10] = -2 / (f - n);
	matrix.ptr()[12] = -(r + l) / (r - l);
	matrix.ptr()[13] = -(t + b) / (t - b);
	matrix.ptr()[14] = -(f + n) / (f - n);
	return matrix;
}

// To delete ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//#include "ComponentCamera.h"
//#include "GameObject.h"
//#include "ModuleScene.h"
//#include "Application.h"
//#include "mmgr/mmgr.h"
//
//
//
//ComponentCamera::ComponentCamera(GameObject* gameobj) : Component(Component::Type::Camera, gameobj)
//{
//	CalculateViewMatrix();
//	//Position = float3(0.0f, 0.0f, 5.0f);
//	//Reference = float3(0.0f, 0.0f, 0.0f);
//}
//
//
//ComponentCamera::~ComponentCamera()
//{
//	LOG("Cleaning camera");
//}
//
//
//
//// -----------------------------------------------------------------
//bool ComponentCamera::Update(float dt)
//{
//	//if (viewport_focus == true)
//	//{
//	//	// Camera ZOOM with MOUSE WHEEL
//	//	ImGuiIO io = ImGui::GetIO();
//	//	float3 newPos(0, 0, 0);
//	//	float speed = io.MouseWheel * 75.0f * dt; // TODO Not hardcoded speed
//	//	if (speed != 0)
//	//	{
//	//		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
//	//			speed *= 2.0f;
//
//	//		newPos -= Z * speed;
//	//		Position += newPos;
//	//	}
//	//	//Reference += newPos;
//
//	//	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
//	//	{
//	//		FocusObject();
//	//	}
//
//	//	// Mouse motion ----------------
//	//	//Free move 
//	//	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
//	//	{
//	//		//Camera motion with WASD, Shift to go faster
//	//		float3 newPos(0, 0, 0);
//	//		float speed = 10.0f * dt;
//
//	//		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
//	//			App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
//	//			speed *= 2.0f;
//
//	//		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
//	//		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;
//
//
//	//		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
//	//		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;
//
//	//		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) newPos -= Y * speed;
//	//		else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) newPos += Y * speed;
//
//	//		Position += newPos;
//	//		Reference += newPos;
//
//	//		RotateWithMouse();
//	//	}
//
//	//	// Orbit move
//	//	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
//	//	{
//	//		RotateWithMouse();
//	//		// To change to the Reference we want to orbit at
//	//		GameObject* object = App->scene->GetSelectedGameObject();
//	//		if (object != nullptr)
//	//		{
//	//			LookAt({ object->center.x, object->center.y,object->center.z });
//	//		}
//	//	}
//
//	//	// Recalculate matrix -------------
//	//	CalculateViewMatrix();
//	//}
//	//return true;
//}
//
//void ComponentCamera::FocusObject(GameObject* object)
//{
//	LOG("Centering camera from [%f,%f,%f]", position.x, position.y, position.z, 'v');
//
//	// To change to the Reference we want to orbit at
//	if (object == nullptr) {
//		object = App->scene->GetSelectedGameObject();
//	}
//
//	if (object != nullptr)
//	{
//		float min = Length(object->bounding_box[9] - position);
//		int face = 9;
//
//		for (int i = 10; i < 13; i++)
//		{
//			float l = Length(object->bounding_box[i] - position);
//			if (l < min) {
//				min = l;
//				face = i;
//			}
//		}
//
//		float3 new_p = { object->bounding_box[face].x,object->bounding_box[face].y,object->bounding_box[face].z };
//		float size = object->size.MaxElement();
//		float offset = Sqrt((size*size) - (size*size / 4));
//		float parent = (object->HasChilds()) ? 1.0f : -1.0f;
//
//		switch (face)
//		{
//		case 9:
//			position = new_p + (c_Z * offset * parent);
//			break;
//		case 10:
//			position = new_p + (c_X * offset* parent);
//			break;
//		case 11:
//			position = new_p - (c_X * offset* parent);
//			break;
//		case 12:
//			position = new_p - (c_Z * offset* parent);
//			break;
//		default:
//			LOG("Could not detect closest face", 'e');
//			break;
//		}
//		//mesh->bounding_box[13] = { Position.x, Position.y, Position.z };
//		//App->resources->bbox_indices[25] = face;
//		//App->resources->GenBoundingBox(mesh);
//
//		LOG("FACE %i", face, 'v');
//		LOG("To [%f,%f,%f]", position.x, position.y, position.z, 'v');
//		LOG("Looking at [%f,%f,%f]", new_p.x, new_p.y, new_p.z, 'v');
//
//		LookAt(new_p);
//
//
//	}
//}
//
//void ComponentCamera::RotateWithMouse() {
//	// Camera rotation with mouse
//	int dx, dy;
//	App->input->GetMouseMotion(dx, dy);
//	dx = -dx;
//	dy = -dy;
//
//	float Sensitivity = 0.25f;
//
//	position -= reference;
//
//	if (dx != 0)
//	{
//		float DeltaX = (float)dx * Sensitivity;
//		
//		M.RotateX(DeltaX);
//		M.RotateY(DeltaX);
//		M.RotateZ(DeltaX);
//
//		//X = glRotat(X, DeltaX, float3(0.0f, 1.0f, 0.0f));
//		//Y = Rotate(Y, DeltaX, float3(0.0f, 1.0f, 0.0f));
//		//Z = Rotate(Z, DeltaX, float3(0.0f, 1.0f, 0.0f));
//	}
//
//	if (dy != 0)
//	{
//		float DeltaY = (float)dy * Sensitivity;
//
//		M.RotateY
//		Y = rotate(Y, DeltaY, X);
//		Z = rotate(Z, DeltaY, X);
//
//		if (Y.y < 0.0f)
//		{
//			Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
//			Y = cross(Z, X);
//		}
//	}
//	position = reference + Z * length(Position);
//}
//// -----------------------------------------------------------------
////void ComponentCamera::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
////{
////	this->Position = Position;
////	this->Reference = Reference;
////
////	Z = normalize(Position - Reference);
////	X = normalize(cross(float3(0.0f, 1.0f, 0.0f), Z));
////	Y = cross(Z, X);
////
////	if (!RotateAroundReference)
////	{
////		this->Reference = this->Position;
////		this->Position += Z * 0.05f;
////	}
////
////	CalculateViewMatrix();
////}
//
//// -----------------------------------------------------------------
//void ComponentCamera::LookAt(const float3 &Spot)
//{
//	reference = Spot;
//
//	M.LookAt(M.Row(0).Normalized(),Spot.Normalized(),M.Row(1).Normalized(),c_Y);
//	/*Z = normalize(Position - Reference);
//	X = normalize(cross(float3(0.0f, 1.0f, 0.0f), Z));
//	Y = cross(Z, X);*/
//
//	CalculateViewMatrix();
//}
//
//
//// -----------------------------------------------------------------
//void ComponentCamera::Move(const float3 &movement)
//{
//	position += movement;
//	reference += movement;
//
//	CalculateViewMatrix();
//}
//
//// -----------------------------------------------------------------
//const float4x4* ComponentCamera::GetViewMatrix()
//{
//	return &view_matrix;
//}
//
//const float4x4* ComponentCamera::GetViewMatrixInverse()
//{
//	return &view_matrix.Inverted();
//}
//
//// -----------------------------------------------------------------
//void ComponentCamera::CalculateViewMatrix()
//{
//	view_matrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -Dot(X, position), -Dot(Y, position), -Dot(Z, position), 1.0f);
//}