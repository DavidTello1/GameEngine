#pragma once
#include "Component.h"
#include "ResourceMesh.h"
#include "Math.h"

#include "glew\include\GL\glew.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* obj, ResourceMesh* mesh);
	virtual ~ComponentMesh();

	ResourceMesh* GetMesh() const { return mesh; }

private:
	ResourceMesh* mesh = nullptr;
};

