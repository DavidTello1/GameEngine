#pragma once
#include "Panel.h"
#include "Math.h"
#include "GameObject.h"
#include "Component.h"
#include "Resource.h"

class Inspector : public Panel
{
public:
	static const uint default_width = 302;
	static const uint default_height = 862;
	static const uint default_pos_x = 978;
	static const uint default_pos_y = 19;

public:
	Inspector();
	~Inspector();

	void Draw();
	void SetPosition(GameObject* Object, float3 Position) { Object->SetLocalPosition(Position); }
	void SetRotation(GameObject* Object, float3 Rotation) { Object->SetRotation(Rotation); }
	void SetScale(GameObject* Object, float3 Scale) { Object->SetLocalScale(Scale); }

	//void GetTriangles();

private:
	void DrawComponents(GameObject* obj, bool res);
	void DrawResource(Resource* res);

private:
	GameObject* obj = nullptr;
	Resource* res = nullptr;
	float3 position, rotation, scale;
	float precision = 0.15f;
	const char* precision_char = "%.2f";

	bool lock_scale = false;

	Component* selected_component;
};