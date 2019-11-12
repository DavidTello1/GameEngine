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

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

ComponentCamera::~ComponentCamera()
{}


void ComponentCamera::RotateWithMouse() {
	// Camera rotation with mouse
	int dx, dy;
	App->input->GetMouseMotion(dx, dy);
	dx = -dx;
	dy = -dy;

	float Sensitivity = 0.25f;

	Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}
	Position = Reference + Z * length(Position);
}
// -----------------------------------------------------------------
void ComponentCamera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ComponentCamera::LookAt(const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ComponentCamera::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ComponentCamera::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ComponentCamera::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
}



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