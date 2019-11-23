#pragma once
#include "Component.h"
#include "ResourceMaterial.h"
#include <string>

#include "glew\include\GL\glew.h"

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(GameObject* obj);
	virtual ~ComponentMaterial();

	void DrawInspector();

	void SetMaterial(ResourceMaterial* Material) { material = Material; }
	ResourceMaterial* GetMaterial() const { return material; }

private:
	ResourceMaterial* material = nullptr;
	static inline Component::Type GetType() { return Component::Type::Material; }

public:
	uint  tex_id = 0;
	char path[256];
	uint width = 0;
	uint height = 0;

};
