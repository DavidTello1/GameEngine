#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

#include "mmgr/mmgr.h"

GLuint GameObject::bounding_box_IBO = 0;

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
}

GameObject::~GameObject()
{
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
}

void GameObject::UnSelect()
{
	hierarchy_flags &= ~ImGuiTreeNodeFlags_Selected;
	is_selected = false;
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

void GameObject::UpdateTransform()
{
	if (flags & ProcessTransformUpdate)
		local_transform = float4x4::FromTRS(translation, rotation_quat, scale);

	if (parent && parent->GetUID() != 0)
	{
		// If things goes strange change order of multiplication
		global_transform = local_transform * parent->GetGlobalTransform();
	}
	else
	{
		global_transform = local_transform;
	}

	for (GameObject* child : childs)
	{
		child->UpdateTransform();
	}

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

	GenerateBoundingBox();
}

void GameObject::ChildDeleted()
{
	if (uid == 0) return;

	if (HasChilds()) 
	{
		aabb.minPoint = childs.back()->aabb.minPoint;
		aabb.maxPoint = childs.back()->aabb.maxPoint;

		GetMinMaxVertex(this, &aabb.minPoint, &aabb.maxPoint);

		GenerateBoundingBox();
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

		aabb.SetNegativeInfinity();
		//b_box.SetFrom(obb);
		aabb.Enclose(obb);
		//b_box.Enclose(mesh->GetMesh()->vertices, mesh->GetMesh()->num_vertices);
	}


	//if (b_box.IsFinite())
	//{
	//	// Already generated bbox -> needs update
	//	if (mesh)
	//	{
	//		obb.SetFrom(mesh->GetMesh()->GetAABB());
	//		obb.Transform(GetGlobalTransform());

	//		b_box.SetNegativeInfinity();
	//		b_box.Enclose(obb);
	//	}
	//	else
	//	{
	//		if (HasChilds())
	//		{
	//			b_box.minPoint = childs.back()->b_box.minPoint;
	//			b_box.maxPoint = childs.back()->b_box.maxPoint;

	//			GetMinMaxVertex(this, &b_box.minPoint, &b_box.maxPoint);

	//			b_box.Enclose(AABB(b_box.minPoint, b_box.maxPoint));
	//		}
	//	}
	//}
	//else
	//{
	//	if (mesh)
	//	{
	//		b_box.Enclose(mesh->GetMesh()->vertices, mesh->GetMesh()->num_vertices);
	//	}
	//}


	/*if (b_box.IsFinite())
	{
		if (mesh)
		{
			b_box.Enclose(mesh->GetMesh()->vertices, mesh->GetMesh()->num_vertices);
		}
		else
		{
			obb.SetFrom(b_box);
			obb.Transform(GetGlobalTransform());

			b_box.SetNegativeInfinity();
			b_box.Enclose(obb);
			
		}
	}
	else
	{
		if (mesh)
		{
			b_box.Enclose(mesh->GetMesh()->vertices, mesh->GetMesh()->num_vertices);
		}

	}*/

	if (bb_VBO == 0) glGenBuffers(1, &bb_VBO);

	aabb.GetCornerPoints(corners);

	glBindBuffer(GL_ARRAY_BUFFER, bb_VBO);
	glBufferData(GL_ARRAY_BUFFER, aabb.NumVertices() * sizeof(float3), corners, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

void GameObject::DeleteBoundingBox()
{
	if (bb_VBO != 0)
	{
		glDeleteFramebuffers(1, &bb_VBO);
		bb_VBO = 0;
	}
	aabb.SetNegativeInfinity();
}

void GameObject::UpdateBoundingBox()
{
	GenerateBoundingBox();
}
