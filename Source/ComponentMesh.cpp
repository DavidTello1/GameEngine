#include "ComponentMesh.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh(GameObject* gameobj) : Component(Component::Type::Mesh, gameobj)
{
}

ComponentMesh::~ComponentMesh()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &TEX);

	if (vertices)
	{
		RELEASE_ARRAY(vertices);
		vertices = nullptr;
	}
	if (indices)
	{
		RELEASE_ARRAY(indices);
		indices = nullptr;
	}
	if (tex_coords)
	{
		RELEASE_ARRAY(tex_coords);
		tex_coords = nullptr;
	}
	if (normals)
	{
		RELEASE_ARRAY(normals);
		normals = nullptr;
	}
}

