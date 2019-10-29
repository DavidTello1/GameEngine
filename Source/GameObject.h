#pragma once

#include "Globals.h"
#include "Component.h"
#include "glew\include\GL\glew.h"

#include "Math.h"
#include <vector>

class ComponentMaterial;
class GameObject
{
public:
	GameObject(const char* name);
	virtual ~GameObject();

	uint GetUID() const { return uid; }
	const char* GetName() const { return name; }
	void SetName(const char* Name) { strcpy_s(this->name, NAME_LENGTH, name); }
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

	void ChildAdded(GameObject * child);

	void SetLocalPosition(const float3& position) { translation = position; }
	void SetLocalScale(const float3& scale) { this->scale = scale; }

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
	HierarchyNode* node;

	std::vector<Component*> components;

	void GenBoundingBox();

	bool HasChilds();

	// Dimensions
	bool is_valid_dimensions = false;
	float3 min_vertex = { 0,0,0 };
	float3 max_vertex = { 0,0,0 };
	float3 center = { 0,0,0 };
	float3 size = { 0,0,0 };

	//0-7 Box vertex
	//8 Box center
	//9-12 Faces center
	float3 bounding_box[13];
	GLuint bb_VBO = 0;
	GLuint bb_IBO = 0;

	bool show_bounding_box = false;
};
