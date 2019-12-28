#include "Application.h"
#include "ModuleSceneBase.h"
#include "ModuleScene.h"
#include "Viewport.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"

#include "SDL/include/SDL.h"
#include "glew/include/GL/glew.h"
#include "mmgr/mmgr.h"

ComponentCamera* viewport_camera = nullptr;

ModuleSceneBase::ModuleSceneBase(bool start_enabled) : Module("SceneBase", start_enabled)
{}

ModuleSceneBase::~ModuleSceneBase()
{}

bool ModuleSceneBase::Start(Config* config)
{
	return true;
}

bool ModuleSceneBase::Update(float dt)
{
	if (viewport_camera == nullptr 
		|| App->editor->focused_panel  == nullptr || App->editor->focused_panel != App->editor->tab_viewport) return true;

	CameraZoom(dt);

	CameraFocusTo();

	CameraFreeMove(dt);

	CameraOrbit(dt);

	if (mouse_picking)
		CameraMousePicking();

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

void ModuleSceneBase::CameraOrbit(float dt)
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		CameraRotateWithMouse(dt);

		GameObject* object = App->scene->GetSelectedGameObject();
		if (object != nullptr)
		{
			viewport_camera->Look(object->aabb.CenterPoint());
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
			float3 v_distance = object->aabb.FaceCenterPoint(0) - viewport_camera->frustum.pos;

			float min = v_distance.Length();
			int face = 0;

			// Check which face of the object is the closest one
			for (int i = 1; i < 6; i++)
			{
				v_distance = object->aabb.FaceCenterPoint(i) - viewport_camera->frustum.pos;
				
				if (v_distance.Length() < min) {
					min = v_distance.Length();
					face = i;
				}
			}

			float size = object->aabb.Size().MaxElement();
			// superformula to define the offset to the object
			float offset = Sqrt((size*size) - (size*size / 4)); 

			//XOR with if is parent and the position (parity) of the face inside the b_box.FaceCenterPoint
			// code reference at commit "Cleaned Focus Camera" +- 280 commit number
			float xor = (object->HasChilds() ^ face % 2 == 0) ? -1.0f : 1.0f;

			if (face < 2)
			{
				viewport_camera->SetPosition(object->aabb.FaceCenterPoint(face) + (float3::unitX * offset * xor));
			}
			else if (face < 4)
			{
				viewport_camera->SetPosition(object->aabb.FaceCenterPoint(face) + (float3::unitY * offset * xor));
			}
			else if (face < 6)
			{
				viewport_camera->SetPosition(object->aabb.FaceCenterPoint(face) + (float3::unitZ * offset * xor));
			}

			viewport_camera->Look(object->aabb.FaceCenterPoint(face));
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

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += viewport_camera->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= viewport_camera->frustum.front * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= viewport_camera->frustum.WorldRight() * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += viewport_camera->frustum.WorldRight() * speed;

		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos -= viewport_camera->frustum.up * speed;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos += viewport_camera->frustum.up * speed;

		viewport_camera->Move(newPos);

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

		viewport_camera->Move(viewport_camera->frustum.front * speed);
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
		viewport_camera->frustum.up = quat.Mul(viewport_camera->frustum.up).Normalized();
		viewport_camera->frustum.front = quat.Mul(viewport_camera->frustum.front).Normalized();
	}

	if (dy != 0)
	{
		Quat quat = Quat::RotateAxisAngle(viewport_camera->frustum.WorldRight(), dy* dt *Sensitivity);
		float3 up = quat.Mul(viewport_camera->frustum.up).Normalized();

		if (up.y > 0.0f)
		{
			viewport_camera->frustum.up = up;
			viewport_camera->frustum.front = quat.Mul(viewport_camera->frustum.front).Normalized();
		}
	}

}

void ModuleSceneBase::CameraMousePicking()
{
	float3 start = { 0,0,0 };
	float3 end = { 0,0,0 };
	if (print_ray.IsFinite()) {
		 start = print_ray.pos;
		 end = print_ray.pos + print_ray.dir.Normalized() * viewport_camera->GetFarPlane();

	}
	bool intersects = false;



	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		float2 pos = App->input->GetMousePosition();

		pos.x = (((float)(( pos.x - (float)App->editor->tab_viewport->pos_x - 7.0f) / (float)App->editor->tab_viewport->width   )) -0.5f) * 2.0f;	
		pos.y = (((float)(( pos.y - (float)App->editor->tab_viewport->pos_y - 26.0f ) / -(float)App->editor->tab_viewport->height )) +0.5f) * 2.0f;

		pos = pos.Clamp(-1.0f, 1.0f);

		
		Ray ray = viewport_camera->frustum.UnProjectFromNearPlane(pos.x, pos.y);
		print_ray = ray;

		// Starts collect
		intersects = App->scene->PickFromRay(ray) != nullptr;

		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_REPEAT)
			LOG("Start [%f,%f,%f]   End [%f,%f,%f]", start.x, start.y, start.z, end.x, end.y, end.z, 'd');
	}

	// Drawiinggg
	Color c = (intersects) ? Color::yellow1 : Color::cyan1;
	

	glLineWidth(1.0f);
	glColor3ub(c.r*255.0f, c.g*255.0f, c.b*255.0f);

	glBegin(GL_LINES);


	glVertex3f(start.x, start.y, start.z);
	glVertex3f(end.x, end.y, end.z);

	glLineWidth(1.0f);
	glColor3ub(255.0f, 255.0f, 255.0f);

	glEnd();
}

bool ModuleSceneBase::Draw()
{
	if (App->editor->is_show_plane) // plane
		DrawGridPlane();

	if (App->editor->is_show_axis) // axis
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