#pragma once
#include "Component.h"
#include <string>

#include "glew\include\GL\glew.h"

struct aiScene;
class ComponentMaterial : public Component
{
public:

	ComponentMaterial(GameObject* obj);
	virtual ~ComponentMaterial();

	void FreeTexture();

public:
	uint  tex_id = 0;
	const char* path;
	uint width = 0;
	uint height = 0;
};
