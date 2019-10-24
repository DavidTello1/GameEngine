#include "Application.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

GameObject::GameObject(const char* name)
{
	uid = App->random->Int();
	strcpy(this->name, name);
}

GameObject::~GameObject()
{
	RELEASE(mesh);
}

Mesh * GameObject::GetMesh()
{
	return this->mesh;
}

void GameObject::SetLocalRotation(const float3& XYZ_euler_rotation)
{
	float3 diff = XYZ_euler_rotation - rotation;
	Quat mod = Quat::FromEulerXYZ(diff.x, diff.y, diff.z);
	rotation_quat = rotation_quat * mod;
	rotation = XYZ_euler_rotation;
}

void GameObject::SetLocalRotation(const Quat& rotation_quat)
{
	this->rotation_quat = rotation_quat;
	rotation = rotation_quat.ToEulerXYZ().Abs();
}


void GameObject::SetLocalTransform(const float4x4 & transform)
{
	transform.Decompose(translation, rotation_quat, scale);
	rotation = rotation_quat.ToEulerXYZ().Abs();
}

void GameObject::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
}
