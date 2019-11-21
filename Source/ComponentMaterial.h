#pragma once
#include "Component.h"
#include <string>

#include "glew\include\GL\glew.h"

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(GameObject* obj);
	virtual ~ComponentMaterial();

	static inline Component::Type GetType() { return Component::Type::Material; }

public:
	uint  tex_id = 0;
	char path[256];
	uint width = 0;
	uint height = 0;
};
