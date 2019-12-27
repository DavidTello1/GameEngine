#pragma once

#include "Globals.h"
#include "Component.h"
#include "UI_Element.h"
#include "glew\include\GL\glew.h"
#include "Hierarchy.h"
#include "ResourceMesh.h"

#include "Math.h"
#include <vector>

class ComponentMaterial;
class ComponentCamera;

//Dealt at scene post update
enum GameObjectFlags
{
	NoFlags					= 0,
	ProcessNewChild			= 1 << 0,
	ProcessDeletedChild		= 1 << 1,
	ProcessTransformUpdate	= 1 << 2,
	UnusedFlagNumber3		= 1 << 3
};

class GameObject
{
	friend class ModuleScene;
	friend class ResourceScene;
	ALIGN_CLASS_TO_16

private:
	GameObject(const char* name, GameObject* Parent);
	virtual ~GameObject();

public:
	void SetName(const char* Name) { strcpy_s(this->name, NAME_LENGTH, Name); }
	const char* GetName() const { return name; }

	uint GetUID() const { return uid; }
	void SetUID(UID id) { uid = id; }
	bool IsActive() { return active; }
	void SetActive(bool act) { active = act; }

	template<typename ComponentType> 
	ComponentType* GetComponent()  { return (ComponentType*)GetComponent(ComponentType::GetType()); }

	Component* GetComponent(Component::Type type);
	Component* AddComponent(Component::Type type, UI_Element::Type UI_type = UI_Element::Type::UNKNOWN);
	bool HasComponent(Component::Type type);
	void DeleteComponent(Component::Type type);

	float3 GetPosition() const { return translation; }
	float3 GetRotation() const { return rotation; }
	Quat GetRotationQ() const { return rotation_quat; }
	float3 GetScale() const { return scale; }
	float4x4 GetLocalTransform() { return local_transform; }
	float4x4 GetGlobalTransform() const { return global_transform; }
	//float3 GetVelocity() const { return velocity; }


	void UpdateTransform();
	void UpdateParentBoundingBox();

	void SetRotation(const float3& XYZ_euler_rotation);
	void SetRotation(const Quat& rotation);
	void SetTransform(const float4x4& transform);

	bool HasChilds() const { return !childs.empty(); } 
	void ChildAdded();
	void ChildDeleted();


	void GenerateBoundingBox();
	void DeleteBoundingBox();
	void UpdateBoundingBox();
	void ResetTransform();
	void GetMinMaxVertex(GameObject * obj, float3 * abs_max, float3 * abs_min);

	void SetLocalPosition(const float3& position);
	void SetLocalScale(const float3& Scale);

	void Move(const float3& velocity);
	void Rotate(float angular_velocity);

private:
	uint uid = 0;
	char name [NAME_LENGTH];
	bool active = true;

	float3 translation = float3::zero;
	Quat rotation_quat = Quat::identity;
	float3 rotation = float3::zero;
	float3 scale = float3::one;
	float4x4 local_transform = math::float4x4::identity;
	float4x4 global_transform = math::float4x4::identity;
	//float3 velocity = float3::zero;

public:
	bool is_static = false;
	bool is_drawn = true;

	// Objects
	int flags = NoFlags;
	GameObject* parent;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;

	// Bounding box
	AABB aabb;
	OBB obb;

	const AABB& GetAABB() { return aabb; }
	const OBB& GetOBB() { return obb; }

	GLuint aabb_VBO = 0;
	GLuint obb_VBO = 0;

	float3 corners[8];

	bool show_aabb = false;
	bool show_obb = false;

	//Hierachy --------------------------------------
public: 
	void Select();
	void UnSelect();
	bool ToggleSelection();

	// Debug
	void LogAction(const char* Action) { LOG("%s node '%s', id: %ld ", Action, name, uid, 'd'); }

public:
	ImGuiTreeNodeFlags hierarchy_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	uint hierarchy_pos = 0;
	bool is_rename = false;
	bool is_selected = false;
};
