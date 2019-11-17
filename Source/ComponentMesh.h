#pragma once
#include "Component.h"
#include "ResourceMesh.h"
#include "Math.h"

#include "glew\include\GL\glew.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* obj/*, ResourceMesh* mesh*/);
	virtual ~ComponentMesh();

	void SetMesh(ResourceMesh* Mesh) { mesh = Mesh; }
	ResourceMesh* GetMesh() const { return mesh; }

	void SetBoundingBox();

private:
	ResourceMesh* mesh = nullptr;
};

