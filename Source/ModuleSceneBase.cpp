#include "Globals.h"
#include "Application.h"
#include "ModuleSceneBase.h"
#include "ModuleResources.h"
#include "Hierarchy.h"
#include "Viewport.h"
#include "GameObject.h"

#include "glew/include/GL/glew.h"
#include "par_shapes.h"
#include "mmgr/mmgr.h"

GameObject* ModuleSceneBase::main_camera_object;
ComponentCamera* ModuleSceneBase::main_camera;

ModuleSceneBase::ModuleSceneBase(bool start_enabled) : Module("SceneBase", start_enabled)
{}

ModuleSceneBase::~ModuleSceneBase()
{}

bool ModuleSceneBase::Start(Config* config)
{
	main_camera_object = App->scene->CreateGameObject("Main Camera");
	main_camera = (ComponentCamera*)main_camera_object->AddComponent(Component::Type::Camera);

	main_camera->Move({ 0, 5.0f, -10.5f });

	return true;
}

bool ModuleSceneBase::Update(float dt)
{
	UpdateMainCamera(dt);

	return true;
}

bool ModuleSceneBase::PostUpdate(float dt)
{
	return true;
}

bool ModuleSceneBase::CleanUp()
{
	LOG("Unloading SceneBase");

	return true;
}


void ModuleSceneBase::UpdateMainCamera(float dt)
{
	if (main_camera == nullptr || main_camera->viewport_focus == false) return;

	CameraZoom(dt);

	CameraFocusTo();

	CameraFreeMove(dt);

	CameraOrbit(dt);
}

void ModuleSceneBase::CameraOrbit(float dt)
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		CameraRotateWithMouse(dt);

		GameObject* object = App->scene->GetSelectedGameObject();
		if (object != nullptr)
		{
			main_camera->Look(object->center);
		}
	}
}

void ModuleSceneBase::CameraFocusTo()
{
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		GameObject* object = App->scene->GetSelectedGameObject();

		if (object != nullptr)
		{
			float3 v_distance = object->bounding_box[9] - main_camera->frustum.pos;

			float min = v_distance.Length();
			int face = 9;

			// Check which face of the object is the closest one
			for (int i = 10; i < 13; i++)
			{
				v_distance = object->bounding_box[i] - main_camera->frustum.pos;
				
				if (v_distance.Length() < min) {
					min = v_distance.Length();
					face = i;
				}
			}

			float size = object->size.MaxElement();
			float offset = Sqrt((size*size) - (size*size / 4)); // superformula to define the offset to the object
			float parent = (object->HasChilds()) ? 1.0f : -1.0f;

			switch (face)
			{
			case 9:
				main_camera->SetPosition(object->bounding_box[face] + (float3::unitZ * offset * parent));
				break;
			case 10:
				main_camera->SetPosition(object->bounding_box[face] + (float3::unitX * offset* parent));
				break;
			case 11:
				main_camera->SetPosition(object->bounding_box[face] - (float3::unitX * offset* parent));
				break;
			case 12:
				main_camera->SetPosition(object->bounding_box[face] - (float3::unitZ * offset* parent));
				break;
			default:
				LOG("Could not detect closest face", 'w');
				break;
			}

			main_camera->Look(object->bounding_box[face]);
		}
	}
}

void ModuleSceneBase::CameraFreeMove(float dt)
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		float3 newPos = float3::zero;
		float speed = 10.0f * dt;

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
			App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
			speed *= 2.0f;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += main_camera->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= main_camera->frustum.front * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= main_camera->frustum.WorldRight() * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += main_camera->frustum.WorldRight() * speed;

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) newPos -= main_camera->frustum.up * speed;
		else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) newPos += main_camera->frustum.up * speed;

		main_camera->Move(newPos);

		CameraRotateWithMouse(dt);
	}
}

void ModuleSceneBase::CameraZoom(float dt)
{
	float speed = ImGui::GetIO().MouseWheel * zoom_speed * dt;

	if (speed != 0)
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
			speed *= 2.0f;

		main_camera->Move(main_camera->frustum.front * speed);
	}
}

void ModuleSceneBase::CameraRotateWithMouse(float dt) {

	int dx, dy;
	App->input->GetMouseMotion(dx, dy);
	dx = -dx;
	dy = -dy;

	float Sensitivity = 0.25f;

	if (dx != 0)
	{
		Quat quat = Quat::RotateY(dx*dt*Sensitivity);
		main_camera->frustum.up = quat.Mul(main_camera->frustum.up).Normalized();
		main_camera->frustum.front = quat.Mul(main_camera->frustum.front).Normalized();
	}

	if (dy != 0)
	{
		Quat quat = Quat::RotateAxisAngle(main_camera->frustum.WorldRight(), dy* dt *Sensitivity);
		float3 up = quat.Mul(main_camera->frustum.up).Normalized();

		if (up.y > 0.0f)
		{
			main_camera->frustum.up = up;
			main_camera->frustum.front = quat.Mul(main_camera->frustum.front).Normalized();
		}
	}

}

bool ModuleSceneBase::Draw()
{
	if (App->editor->show_plane) // plane
		DrawGridPlane();

	if (App->editor->show_axis) // axis
		DrawAxis();

	return true;
}


void ModuleSceneBase::DrawGridPlane()
{
	glLineWidth(0.25f);

	glBegin(GL_LINES);
	glColor3ub(100, 100, 100);
	float d = plane_length;
	for (float i = -d; i <= d; ++i)
	{
		glVertex3f(i, 0.f, 0.f);
		glVertex3f(i, 0, d);

		glVertex3f(0.f, 0.f, i);
		glVertex3f(d, 0, i);

		glVertex3f(i, 0.f, 0.f);
		glVertex3f(i, 0, -d);

		glVertex3f(0.f, 0.f, i);
		glVertex3f(-d, 0, i);
	}
	glEnd();
}

void ModuleSceneBase::DrawAxis()
{
	glLineWidth(3.0f);
	glBegin(GL_LINES);

	// x
	glColor3ub(255, 0, 0);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(axis_length, 0.0f, 0.0f);

	//y
	glColor3ub(0, 255, 0);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, axis_length, 0.0f);

	// z
	glColor3ub(0, 0, 255);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, axis_length);

	glColor3ub(255, 255, 255);


	glEnd();
}