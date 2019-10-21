#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include <list>
#include "Math.h"

class GameObject
{
public:
	ALIGN_CLASS_TO_16

	GameObject(const char* name);
	virtual ~GameObject();

	//uint GetUID() const { return uid; }

	//float3 GetLocalPosition() const { return translation; }
	//float3 GetGlobalPosition() const { return transform_global.TranslatePart(); }
	//float3 GetLocalRotation() const { return rotation; }
	//Quat GetLocalRotationQ() const { return rotation_quat; }
	//float3 GetLocalScale() const { return scale; }

	//void SetLocalRotation(const float3& XYZ_euler_rotation);
	//void SetLocalRotation(const Quat& rotation);
	//void SetLocalTransform(const float4x4& transform);

	//void SetLocalPosition(const float3& position) { translation = position; }
	//void SetLocalScale(const float3& scale) { this->scale = scale; }
	//
	//void Move(const float3& velocity) { translation += velocity; }
	//void Rotate(float angular_velocity){ rotation_quat = rotation_quat * Quat::RotateY(angular_velocity); }

	//const float4x4& GetGlobalTransformation() const { return transform_global; }
	//const float* GetOpenGLGlobalTransform() const { return transform_global.Transposed().ptr(); }

	//float3 GetVelocity() const { return velocity; }

private:
	bool active = true;

	uint uid = 0;

	//Quat rotation_quat = Quat::identity;
	//float3 rotation = float3::zero;

	//float4x4 transform_global;
	//float4x4 original_transform;

	//float3 last_translation = float3::zero;
	//float3 translation = float3::zero;

	//float3 scale = float3::one;

	//float3 velocity = float3::zero;

public:
	const char* name;

	mutable bool visible = false;
	bool flag_for_removal = false;

};

#endif
