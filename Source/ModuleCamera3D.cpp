#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Imgui/imgui.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ModuleScene.h"
#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module("Camera3D", start_enabled)
{
	CalculateViewMatrix();

	//X = vec3(1.0f, 0.0f, 0.0f);
	//Y = vec3(0.0f, 1.0f, 0.0f);
	//Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start(Config* config)
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Update(float dt)
{
	if (viewport_focus == true)
	{
		// Camera ZOOM with MOUSE WHEEL
		ImGuiIO io = ImGui::GetIO();
		vec3 newPos(0, 0, 0);
		float speed = io.MouseWheel * 75.0f * dt; // TODO Not hardcoded speed
		if (speed != 0)
		{
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
				speed *= 2.0f;

			newPos -= Z * speed;
			Position += newPos;
		}
		//Reference += newPos;

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			LOG("Centering camera from [%f,%f,%f]", Position.x, Position.y, Position.z,'v');
			// To change to the Reference we want to orbit at
			GameObject* object = App->scene->GetSelectedGameObject();
			if (object != nullptr)
			{
				float3 c = { Position.x,Position.y,Position.z };
				float3 p = object->bounding_box[9] - c;

				float l = length(vec3(p.x, p.y, p.z));
				float min = l;
				int face = 9;

				for (int i = 10; i < 13; i++)
				{
					p = object->bounding_box[i] - c;
					l = length(vec3(p.x, p.y, p.z));
					if (l < min) {
						min = l;
						face = i;
					}
				}

				vec3 new_p = { object->bounding_box[face].x,object->bounding_box[face].y,object->bounding_box[face].z };
				float size = object->size.MaxElement();
				float offset = Sqrt((size*size) - (size*size / 4));
				switch (face)
				{
				case 9:
					if (object->HasChilds())
						Position = new_p + c_Z * offset;
					else
						Position = new_p - c_Z * offset;
					break;
				case 10:
					if (object->HasChilds())
						Position = new_p + c_X * offset;
					else
						Position = new_p - c_X * offset;
					break;
				case 11:
					if (object->HasChilds())
						Position = new_p - c_X * offset;
					else
						Position = new_p + c_X * offset;
					break;
				case 12:
					if (object->HasChilds())
						Position = new_p - c_Z * offset;
					else
						Position = new_p + c_Z * offset;
					break;
				default:
					LOG("Could not detect closest face", 'e');
					break;
				}
				//mesh->bounding_box[13] = { Position.x, Position.y, Position.z };
				//App->resources->bbox_indices[25] = face;
				//App->resources->GenBoundingBox(mesh);

				LOG("FACE %i", face, 'v');
				LOG("To [%f,%f,%f]", Position.x, Position.y, Position.z, 'v');
				LOG("Looking at [%f,%f,%f]", new_p.x, new_p.y, new_p.z, 'v');

				LookAt(new_p);
				
				
			}
		}

		// Mouse motion ----------------
		//Free move 
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			//Camera motion with WASD, Shift to go faster
			vec3 newPos(0, 0, 0);
			float speed = 10.0f * dt;

			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
				App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
				speed *= 2.0f;

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) newPos -= Y * speed;
			else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) newPos += Y * speed;

			Position += newPos;
			Reference += newPos;

			RotateWithMouse();
		}

		// Orbit move
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
		{
			RotateWithMouse();
			// To change to the Reference we want to orbit at
			GameObject* object = App->scene->GetSelectedGameObject();
			if (object != nullptr)
			{
				LookAt({ object->center.x, object->center.y,object->center.z });
			}
		}

		// Recalculate matrix -------------
		CalculateViewMatrix();
	}
	return true;
}

void ModuleCamera3D::RotateWithMouse() {
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
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}