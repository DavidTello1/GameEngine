#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"

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
}

GameObject::~GameObject()
{
	for (uint i = 0; i < components.size(); ++i)
		delete components[i];

	components.clear();
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
void GameObject::LogAction(const char * Action)
{
	LOG("%s node '%s', id: %ld ", Action, name, uid, 'd');
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

void GameObject::GetMinMaxVertex(GameObject* obj, float3* abs_max, float3* abs_min)
{
	if (!obj->HasChilds())
	{
		if (!obj->is_valid_dimensions) return;
		if (obj->min_vertex.x < abs_min->x) abs_min->x = obj->min_vertex.x;
		if (obj->min_vertex.y < abs_min->y) abs_min->y = obj->min_vertex.y;
		if (obj->min_vertex.z < abs_min->z) abs_min->z = obj->min_vertex.z;

		if (obj->max_vertex.x > abs_max->x) abs_max->x = obj->max_vertex.x;
		if (obj->max_vertex.y > abs_max->y) abs_max->y = obj->max_vertex.y;
		if (obj->max_vertex.z > abs_max->z) abs_max->z = obj->max_vertex.z;
	}
	else {
		for (GameObject* child : obj->childs)
		{
			GetMinMaxVertex(child, abs_max, abs_min);
		}

	}
}

// child to be replaced by childs.back()
void GameObject::ChildAdded()
{
	// Checking if new child node is not an empty node and parent not root node
	if (childs.back()->uid == 0) {
		LOG("Child is root node", 'e');
		return;
	}
	if (uid == 0 || !childs.back()->is_valid_dimensions) return;

	min_vertex = childs.back()->min_vertex;
	max_vertex = childs.back()->max_vertex;

	GetMinMaxVertex(this, &min_vertex, &max_vertex);

	GenBoundingBox();

}

void GameObject::GenBoundingBox()
{
	//Bounding box
	float3 min = min_vertex;
	float3 max = max_vertex;

	bounding_box[0] = { min.x,min.y,min.z };
	bounding_box[1] = { min.x,min.y,max.z };
	bounding_box[2] = { max.x,min.y,max.z };
	bounding_box[3] = { max.x,min.y,min.z };

	bounding_box[4] = { min.x,max.y,min.z };
	bounding_box[5] = { min.x,max.y,max.z };
	bounding_box[6] = { max.x,max.y,max.z };
	bounding_box[7] = { max.x,max.y,min.z };

	float3 c = (min_vertex + max_vertex) / 2;
	center = c; // gameobject variable
	bounding_box[8] = center;

	bounding_box[9] = { c.x,c.y,min.z };  //Face 0437 center
	bounding_box[10] = { min.x,c.y,c.z };  //Face 0415 center
	bounding_box[11] = { max.x,c.y,c.z }; //Face 3726 center
	bounding_box[12] = { c.x,c.y,max.z }; //Face 1256 center
	//mesh_component->bounding_box[13] = { 0,0,0 }; // camera
	//mesh_component->bounding_box[11] = { c.x,max.y,c.z }; //Face 4567 center (top face)
	//mesh_component->bounding_box[14] = { c.x,min.y,c.z }; //Face 0123 center (bot face)



	size = max_vertex - min_vertex;


	//Deleting buffers if any
	if (bb_VBO == 0)
	{
		glGenBuffers(1, &bb_VBO);
		//glDeleteFramebuffers(1, &bb_VBO);
		//bb_VBO = 0;
	}
	if (bb_IBO == 0)
	{
		glGenBuffers(1, &bb_IBO);
		//glDeleteBuffers(1, &bb_IBO);
		//bb_IBO = 0;
	}

	// VBO
	
	glBindBuffer(GL_ARRAY_BUFFER, bb_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bounding_box), bounding_box, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//IBO
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bb_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(App->resources->bbox_indices), App->resources->bbox_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	LOG("BoundingBox vertices", 'd');

	for (int i = 0; i < 8; i++)
	{
		LOG("{ %f , %f , %f }", bounding_box[i].x, bounding_box[i].y, bounding_box[i].z, 'd');
	}
}

bool GameObject::HasChilds()
{
	return !childs.empty();
}