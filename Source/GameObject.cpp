#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"

#include "mmgr/mmgr.h"

GameObject::GameObject(const char* name)
{
	uid = App->random->Int();
	strcpy_s(this->name, name);
}

GameObject::~GameObject()
{
	for (uint i = 0; i < components.size(); i++)
		delete components[i];

	components.clear();
}

Component* GameObject::GetComponent(Component::Type type)
{
	if (active)
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == type)
				return components[i];
		}
	}

	return nullptr;
}

Component* GameObject::AddComponent(Component::Type type)
{
	Component* new_component;

	//if (GetComponent(type) != nullptr)
	//{
	//	LOG("GameObject already has component", 'e');
	//	return nullptr;
	//}
	if (type == Component::Type::Unknown)
	{
		LOG("Unknown Component", 'e');
		return nullptr;
	}
	else if (type == Component::Type::Mesh)
	{
		new_component = new ComponentMesh(this);
		components.push_back(new_component);
		return new_component;
	}
	else if (type == Component::Type::Material)
	{
		new_component = new ComponentMaterial(this);
		components.push_back(new_component);
		return new_component;
	}
	else if (type == Component::Type::Renderer)
	{
		new_component = new ComponentRenderer(this);
		components.push_back(new_component);
		return new_component;
	}
	return nullptr;
}

bool GameObject::HasComponent(Component::Type type)
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return true;
	}
	return false;
}

void GameObject::DeleteComponent(Component::Type type)
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
		{
			RELEASE(components[i]);
			components.erase(components.begin() + i);
			break;
		}
	}
}


// ---------------------
void GameObject::SetRotation(const float3& XYZ_euler_rotation)
{
	float3 diff = XYZ_euler_rotation - rotation;
	Quat mod = Quat::FromEulerXYZ(diff.x, diff.y, diff.z);
	rotation_quat = rotation_quat * mod;
	rotation = XYZ_euler_rotation;
}

void GameObject::SetRotation(const Quat& rotation_quat)
{
	this->rotation_quat = rotation_quat;
	rotation = rotation_quat.ToEulerXYZ().Abs();
}


void GameObject::SetTransform(const float4x4 & transform)
{
	transform.Decompose(translation, rotation_quat, scale);
	rotation = rotation_quat.ToEulerXYZ().Abs();
}
