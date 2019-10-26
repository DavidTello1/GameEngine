#pragma once
#include "Component.h"

class ComponentMesh;
class ComponentRenderer : public Component
{
public:

	ComponentRenderer(GameObject* obj);
	virtual ~ComponentRenderer();

	void Draw();

private:
	void DrawMesh(ComponentMesh& mesh) const;
	void DrawFaceNormals();
	void DrawVertexNormals();

public:
	bool show_vertex_normals = false;
	bool show_face_normals = false;
	bool show_checkers = false;
	bool show_wireframe = false;

private:
	float normals_size = 0.5f;
};
