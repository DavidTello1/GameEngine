#pragma once

#include "Globals.h"
#include "Component.h"
#include "glew\include\GL\glew.h"
#include "Hierarchy.h"

#include "Math.h"
#include <vector>

class ComponentMaterial;

//Dealt at scene post update
enum GameObjectFlags
{
	NoFlags				= 0,
	ProcessNewChild		= 1 << 0,
	ProcessDeletedChild	= 1 << 1,
	UnusedFlagNumber2	= 1 << 2,
	UnusedFlagNumber3	= 1 << 3
};

class GameObject
{
	friend class ModuleScene;
	ALIGN_CLASS_TO_16

private:
	GameObject(const char* name, GameObject* Parent);
	virtual ~GameObject();

public:
	void SetName(const char* Name) { strcpy_s(this->name, NAME_LENGTH, Name); }
	const char* GetName() const { return name; }

	uint GetUID() const { return uid; }
	bool IsActive() { return active; }

	Component* GetComponent(Component::Type type);
	Component* AddComponent(Component::Type type);
	bool HasComponent(Component::Type type);
	void DeleteComponent(Component::Type type);

	float3 GetPosition() const { return translation; }
	float3 GetRotation() const { return rotation; }
	Quat GetRotationQ() const { return rotation_quat; }
	float3 GetScale() const { return scale; }
	float4x4 GetLocalTransform() {return local_transform; }
	//float3 GetVelocity() const { return velocity; }

	void SetRotation(const float3& XYZ_euler_rotation);
	void SetRotation(const Quat& rotation);
	void SetTransform(const float4x4& transform);

	bool HasChilds() { return !childs.empty(); }
	void ChildAdded();
	void ChildDeleted();

	void GetMinMaxVertex(GameObject * obj, float3 * abs_max, float3 * abs_min);
	void GenBoundingBox(bool to_delete = false);

	void SetLocalPosition(const float3& position) { translation = position; }
	void SetLocalScale(const float3& Scale) { scale = Scale; }

	void Move(const float3& velocity) { translation += velocity; }
	void Rotate(float angular_velocity) { rotation_quat = rotation_quat * Quat::RotateY(angular_velocity); }

private:
	uint uid = 0;
	char name [NAME_LENGTH];
	bool active = true;

	float3 translation = float3::zero;
	Quat rotation_quat = Quat::identity;
	float3 rotation = float3::zero;
	float3 scale = float3::one;
	float4x4 local_transform = math::float4x4::identity;
	//float3 velocity = float3::zero;

public:

	// Objects
	int flags = NoFlags;
	GameObject* parent;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;

	// Bounding box
	AABB b_box;

	GLuint bb_VBO = 0;

	static GLuint bounding_box_IBO;

	bool show_bounding_box = false;

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
