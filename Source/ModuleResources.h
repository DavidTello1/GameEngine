#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include "Hierarchy.h"
#include <vector>

class Resource;
class ResourceMesh;
class ResourceMaterial;

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	void ImportResource(const char* path);
	void LoadResource(const char* path);
	void UnLoadResource();

	void GenVBO(ComponentMesh * mesh_component);
	void GenIBO(ComponentMesh * mesh_component);
	void GenTexture(ComponentMesh * mesh_component);

	void ImportMesh(aiMesh* mesh, ComponentMesh* mesh_component);
	GLuint ImportTexture(int width, int height, int internal_format, int format, unsigned char * image);

	void CreateShape(const shape_type & type, int slices, int stacks, float x = 0.0f, float y = 0.0f, float z = 0.0f, float radius = 0.5f, GameObject* parent = nullptr);

public:
	GLuint checker_texture;

	const char* GetShapeName(shape_type type) { return shape_to_string[type];}

private:
	const char* shape_to_string[shape_type::UNKNOWN] =
	{ "Cylinder","Cone", "Torus", "Sphere", "Bottle", "Knot",
		"Hemisphere", "Plane", "Icosahedron", "Dodecahedron",
		"Octahedron", "Tetrahedron", "Cube", "Rock" };

	uint tex_height, tex_width;

	void MakeCheckersTexture();

public:
	GLuint bbox_indices[24] =
	{
		0,1,2,3,0,3,1,2,
		4,5,6,7,4,7,5,6,
		0,4,1,5,2,6,3,7
	};
};

#endif