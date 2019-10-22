#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "Mesh.h"

#include "Math.h"
#include <list>

class GameObject
{
public:
	ALIGN_CLASS_TO_16

	GameObject(const char* name);
	virtual ~GameObject();

	uint GetUID() const { return uid; }
	const char* GetName() const { return name; }
	void SetName(const char* Name) { name = Name; }
	bool IsActive() { return active; }

	// Getters
	float3 GetLocalPosition() const { return translation; }
	float3 GetGlobalPosition() const { return transform_global.TranslatePart(); }
	float3 GetLocalRotation() const { return rotation; }
	Quat GetLocalRotationQ() const { return rotation_quat; }
	float3 GetLocalScale() const { return scale; }
	float3 GetVelocity() const { return velocity; }
	const float4x4& GetGlobalTransformation() const { return transform_global; }
	const float* GetOpenGLGlobalTransform() const { return transform_global.Transposed().ptr(); }

	// Setters
	void SetLocalRotation(const float3& XYZ_euler_rotation);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalTransform(const float4x4& transform);

	void SetLocalPosition(const float3& position) { translation = position; }
	void SetLocalScale(const float3& scale) { this->scale = scale; }
	
	// Functions
	void Move(const float3& velocity) { translation += velocity; }
	void Rotate(float angular_velocity){ rotation_quat = rotation_quat * Quat::RotateY(angular_velocity); }



private:
	uint uid = 0;
	const char* name;
	bool active = true;

	Quat rotation_quat = Quat::identity;
	float3 rotation = float3::zero;
	float4x4 transform_global;
	float3 translation = float3::zero;
	float3 scale = float3::one;
	float3 velocity = float3::zero;


public:
	mutable bool visible = false;
	std::vector<Mesh*> meshes;

};
#endif
