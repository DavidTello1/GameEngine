#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "GameObject.h"
#include "Component.h"
#include <vector>

#include "glew\include\GL\glew.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

enum shape_type {
	CYLINDER,
	CONE,
	TORUS,
	SPHERE,
	BOTTLE,
	KNOT,
	HEMISPHERE,
	PLANE,
	ICOSAHEDRON,
	DODECAHEDRON,
	OCTAHEDRON,
	TETRAHEDRON,
	CUBE,
	ROCK,

	UNKNOWN
};

struct aiMesh;
struct aiScene;
class ComponentMesh;
class ComponentMaterial;

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	Component::Type GetType(const char* path);

	void LogImageInfo();
	void LoadResource(const char* path, Component::Type type = Component::Type::Unknown, bool use = false);
	void UnLoadResource();

	void ImportMesh(aiMesh* mesh, ComponentMesh* mesh_component);
	void GenVBO(ComponentMesh * mesh_component);
	void GenIBO(ComponentMesh * mesh_component);
	void GenTexture(ComponentMesh * mesh_component);
	GLuint ImportTexture(int width, int height, int internal_format, int format, unsigned char * image);
	//void ImportModel(aiMesh* mesh, ComponentMesh* mesh_component, aiScene* material, ComponentMaterial* material_component);

	void CreateShape(const shape_type & type, int slices, int stacks, float x, float y, float z, float radius = 0.5f, uint parent_id = 0);

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
};

#endif