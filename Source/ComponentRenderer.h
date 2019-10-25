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

public:
	bool show_vertex_normals = false;
	bool show_face_normals = false;
	bool show_checkers = false;
	bool show_bounding_box = true;

};
