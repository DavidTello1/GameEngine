#include "ComponentMesh.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh(GameObject* gameobj) : Component(Component::Type::Mesh, gameobj)
{
	object = gameobj;

	if (mesh == nullptr)
	{
		LOG("Error setting bounding box, mesh is nullptr", 'e');
		return;
	}

	// Bounding box setting up
	if (mesh->num_vertices > 0 && mesh->vertices != nullptr)
	{
		object->min_vertex.x = mesh->vertices[0].x;
		object->min_vertex.y = mesh->vertices[0].y;
		object->min_vertex.z = mesh->vertices[0].z;

		object->max_vertex.x = mesh->vertices[0].x;
		object->max_vertex.y = mesh->vertices[0].y;
		object->max_vertex.z = mesh->vertices[0].z;

		for (uint i = 0; i < mesh->num_vertices; ++i)
		{
			float x = mesh->vertices[i].x;
			float y = mesh->vertices[i].y;
			float z = mesh->vertices[i].z;

			if (x < object->min_vertex.x) object->min_vertex.x = x;
			if (y < object->min_vertex.y) object->min_vertex.y = y;
			if (z < object->min_vertex.z) object->min_vertex.z = z;

			if (x > object->max_vertex.x) object->max_vertex.x = x;
			if (y > object->max_vertex.y) object->max_vertex.y = y;
			if (z > object->max_vertex.z) object->max_vertex.z = z;
		}
	}
	else 
	{
		LOG("Mesh has no vertices - Cannot create bounding box", 'e');
		return;
	}
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::SetBoundingBox()
{

}