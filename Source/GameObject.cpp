#include "Application.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "Image.h"
#include "Text.h"
#include "Canvas.h"

#include "mmgr/mmgr.h"


GameObject::GameObject(const char* name, GameObject* Parent)
{
	uid = App->random->Int();
	strcpy_s(this->name, name);

	is_selected = false;
	is_rename = false;

	// Leaf flags
	hierarchy_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
	LogAction("Created object");

	parent = Parent;
	if (parent != nullptr)
	{
		parent->childs.push_back(this);
		parent->hierarchy_flags &= ~ImGuiTreeNodeFlags_Leaf;
		parent->flags |= ProcessNewChild;
		parent->LogAction("To parent");
	}

	aabb.SetNegativeInfinity();
	aabb.padding = 0;
	aabb.padding2 = 0;

	obb.SetNegativeInfinity();
}

GameObject::~GameObject()
{
	DeleteBoundingBox();

	// Delete from parent
	if (parent)
	{
		for (uint i = 0; i < parent->childs.size(); i++)
		{
			if (parent->childs[i] == this)
			{
				parent->childs.erase(parent->childs.begin() + i);
				break;
			}
		}
		if (!parent->HasChilds())
			parent->hierarchy_flags |= ImGuiTreeNodeFlags_Leaf;

		parent->flags |= ProcessDeletedChild;
	}

	for (uint i = 0; i < components.size(); ++i)
		delete components[i];

	components.clear();

	LogAction("Deleted");
}


void GameObject::Select()
{
	hierarchy_flags |= ImGuiTreeNodeFlags_Selected;
	is_selected = true;
	App->scene->is_selecting = true;
}

void GameObject::UnSelect()
{
	hierarchy_flags &= ~ImGuiTreeNodeFlags_Selected;
	is_selected = false;
	App->scene->is_selecting = false;
}

bool GameObject::ToggleSelection() // Toggles the state of the node, returns current state after toggled
{
	if (!is_selected) // not selected
	{
		Select();
	}
	else // selected
	{
		UnSelect();
	}

	return is_selected;
}

Component* GameObject::GetComponent(Component::Type type, UI_Element::Type UI_type)
{
	if (active)
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->type == type)
			{
				if (type == Component::Type::UI_Element && UI_type != UI_Element::Type::UNKNOWN)
				{
					UI_Element* ui_comp = (UI_Element*)components[i];
					if (ui_comp->GetType() == UI_type)
						return components[i];
				}
				else
					return components[i];
			}
		}
	}
	else
	{
		LOG("GameObject not active - cannot Get Component", 'd');
	}

	return nullptr;
}

Component* GameObject::AddComponent(Component::Type type, UI_Element::Type UI_type)
{
	Component* new_component;

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
	else if (type == Component::Type::Camera)
	{
		new_component = new ComponentCamera(this);
		components.push_back(new_component);
		//App->resources->LoadResource("Assets/camera_mesh.fbx", Component::Type::Mesh, true, this);
		return new_component;
	}
	else if (type == Component::Type::UI_Element)
	{
		if (UI_type == UI_Element::Type::CANVAS)
		{
			new_component = new Canvas(this, UI_type);
			components.push_back(new_component);
			return new_component;
		}
		if (UI_type == UI_Element::Type::IMAGE)
		{
			new_component = new Image(this, UI_type);
			components.push_back(new_component);
			return new_component;
		}
		if (UI_type == UI_Element::Type::TEXT)
		{
			new_component = new Text(this, UI_type);
			components.push_back(new_component);
			return new_component;
		}

	}
	return nullptr;
}

bool GameObject::HasComponent(Component::Type type, UI_Element::Type UI_type)
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type)
		{
			if (type == Component::Type::UI_Element && UI_type != UI_Element::Type::UNKNOWN)
			{
				UI_Element* ui_comp = (UI_Element*)components[i];
				if (ui_comp->GetType() == UI_type)
					return true;
			}
			else
				return true;
		}
	}
	return false;
}

void GameObject::DeleteComponent(Component::Type type, UI_Element::Type UI_type)
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type)
		{
			if (type == Component::Type::UI_Element && UI_type != UI_Element::Type::UNKNOWN)
			{
				UI_Element* ui_comp = (UI_Element*)components[i];
				if (ui_comp->GetType() != UI_type)
					continue;
			}
			RELEASE(components[i]);
			components.erase(components.begin() + i);
			break;
		}
	}
}

std::vector<Component*> GameObject::GetAllComponentsOfType(Component::Type type, UI_Element::Type UI_type)
{
	std::vector<Component*> comps;

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type)
		{
			if (type == Component::Type::UI_Element && UI_type != UI_Element::Type::UNKNOWN)
			{
				UI_Element* ui_comp = (UI_Element*)components[i];
				if (ui_comp->GetType() == UI_type)
					comps.push_back(components[i]);
			}
			else
				comps.push_back(components[i]);
		}
	}

	return comps;
}

void GameObject::UpdateTransform()
{
	if (flags & ProcessTransformUpdate)
		local_transform = float4x4::FromTRS(translation, rotation_quat, scale);

	if (parent && parent->GetUID() != 0)
	{
		// If things goes strange change order of multiplication
		global_transform = parent->GetGlobalTransform() * local_transform;
	}
	else
	{
		global_transform = local_transform;
	}

	for (GameObject* child : childs)
	{
		child->UpdateTransform();
	}

	// Too slow, needs better structure
	ComponentCamera* cam = GetComponent<ComponentCamera>();
	if (cam)
	{
		cam->SetPosition(translation);
		cam->frustum.front = GetGlobalTransform().WorldZ();
		cam->frustum.up = GetGlobalTransform().WorldY();
		cam->UpdateMatrices();
	}

	UpdateBoundingBox();
}

void GameObject::UpdateParentBoundingBox()
{
	if (GetUID() == 0) return;

	UpdateBoundingBox();
	if (parent)
		parent->UpdateParentBoundingBox();
}

// ---------------------
void GameObject::SetRotation(const float3& XYZ_euler_rotation)
{
	float3 diff = (XYZ_euler_rotation - rotation);
	Quat mod = Quat::FromEulerXYZ(diff.x, diff.y, diff.z);
	rotation_quat = rotation_quat * mod;
	rotation = XYZ_euler_rotation;

	//rotation_quat = rotation_quat *Quat::FromEulerXYZ(XYZ_euler_rotation.x, XYZ_euler_rotation.y, XYZ_euler_rotation.z);
	flags |= ProcessTransformUpdate;
}

void GameObject::SetRotation(const Quat& rotation_quat)
{
	this->rotation_quat = rotation_quat;
	rotation = rotation_quat.ToEulerXYZ().Abs();
	flags |= ProcessTransformUpdate;
}


void GameObject::SetTransform(const float4x4 & transform)
{
	transform.Decompose(translation, rotation_quat, scale);
	rotation = rotation_quat.ToEulerXYZ().Abs();
	flags |= ProcessTransformUpdate;
}

void GameObject::ResetTransform()
{
	SetTransform(float4x4::identity);
}

void GameObject::GetMinMaxVertex(GameObject* obj, float3* abs_max, float3* abs_min)
{
	if (!obj->HasChilds())
	{
		if (!obj->aabb.IsFinite()) return;
		if (obj->aabb.minPoint.x < abs_min->x) abs_min->x = obj->aabb.minPoint.x;
		if (obj->aabb.minPoint.y < abs_min->y) abs_min->y = obj->aabb.minPoint.y;
		if (obj->aabb.minPoint.z < abs_min->z) abs_min->z = obj->aabb.minPoint.z;

		if (obj->aabb.maxPoint.x > abs_max->x) abs_max->x = obj->aabb.maxPoint.x;
		if (obj->aabb.maxPoint.y > abs_max->y) abs_max->y = obj->aabb.maxPoint.y;
		if (obj->aabb.maxPoint.z > abs_max->z) abs_max->z = obj->aabb.maxPoint.z;
	}
	else {
		for (GameObject* child : obj->childs)
		{
			GetMinMaxVertex(child, abs_max, abs_min);
		}
	}
}

void GameObject::SetLocalPosition(const float3 & position)
{
	translation = position; 
	flags |= ProcessTransformUpdate;
}

void GameObject::SetLocalScale(const float3 & Scale)
{
	scale = Scale;
	flags |= ProcessTransformUpdate;
}

void GameObject::Move(const float3 & velocity)
{
	translation += velocity;
	flags |= ProcessTransformUpdate;
}

void GameObject::Rotate(float angular_velocity)
{
	rotation_quat = rotation_quat * Quat::RotateY(angular_velocity);
	flags |= ProcessTransformUpdate;
}

void GameObject::ChildAdded()
{
	// Checking if new child node is not an empty node and parent not root node
	if (childs.back()->uid == 0) {
		LOG("Child is root node", 'e');
		return;
	}
	if (uid == 0 || !childs.back()->aabb.IsFinite()) return;

	aabb.minPoint = childs.back()->aabb.minPoint;
	aabb.maxPoint = childs.back()->aabb.maxPoint;

	GetMinMaxVertex(this, &aabb.minPoint, &aabb.maxPoint);

	UpdateBoundingBox();
}

void GameObject::ChildDeleted()
{
	if (uid == 0) return;

	if (HasChilds()) 
	{
		aabb.minPoint = childs.back()->aabb.minPoint;
		aabb.maxPoint = childs.back()->aabb.maxPoint;

		GetMinMaxVertex(this, &aabb.minPoint, &aabb.maxPoint);

		UpdateBoundingBox();
	}
	else 
	{
		DeleteBoundingBox();
	}
}

void GameObject::GenerateBoundingBox()
{
	const ComponentMesh* mesh = (ComponentMesh*)GetComponent(Component::Type::Mesh);


	if (mesh)
	{
		obb.SetFrom(mesh->GetMesh()->GetAABB());
		obb.Transform(GetGlobalTransform());

		aabb.SetFrom(obb);
	}
	else if (HasChilds())
	{
		aabb.minPoint = childs.back()->aabb.minPoint;
		aabb.maxPoint = childs.back()->aabb.maxPoint;

		GetMinMaxVertex(this, &aabb.minPoint, &aabb.maxPoint);

		if (obb_VBO != 0)
		{
			glDeleteFramebuffers(1, &obb_VBO);
			obb_VBO = 0;
		}
		obb.SetNegativeInfinity();

		//obb.SetFrom(aabb);
		//obb.Transform(GetGlobalTransform());
	}

	//AABB
	if (aabb_VBO == 0) glGenBuffers(1, &aabb_VBO);

	aabb.GetCornerPoints(corners);

	glBindBuffer(GL_ARRAY_BUFFER, aabb_VBO);
	glBufferData(GL_ARRAY_BUFFER, aabb.NumVertices() * sizeof(float3), corners, GL_STATIC_DRAW);

	//OBB
	if (obb_VBO == 0) glGenBuffers(1, &obb_VBO);

	obb.GetCornerPoints(corners);

	glBindBuffer(GL_ARRAY_BUFFER, obb_VBO);
	glBufferData(GL_ARRAY_BUFFER, obb.NumVertices() * sizeof(float3), corners, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

void GameObject::DeleteBoundingBox()
{
	if (aabb_VBO != 0)
	{
		glDeleteFramebuffers(1, &aabb_VBO);
		aabb_VBO = 0;
	}
	aabb.SetNegativeInfinity();

	if (obb_VBO != 0)
	{
		glDeleteFramebuffers(1, &obb_VBO);
		obb_VBO = 0;
	}
	obb.SetNegativeInfinity();
}

void GameObject::UpdateBoundingBox()
{
	GenerateBoundingBox();
}
