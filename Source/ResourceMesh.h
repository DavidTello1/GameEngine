#pragma once
#include "Resource.h"
#include "Math.h"
#include <string>

#include "glew\include\GL\glew.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

struct aiMesh;
struct par_shapes_mesh_s;

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

class Resource;
class ResourceMesh : public Resource
{
public:
	ResourceMesh(UID id);
	virtual ~ResourceMesh();

	bool Import(const char* file, const char* path, const char* buffer);
	bool Load(const char* exported_file, ResourceMesh* mesh);
	bool UnLoad(ResourceMesh* mesh);

	void Draw();

private:
	void GenVBO();
	void GenIBO();
	void GenTexture();

public:
	float3* vertices = nullptr;	// Array of vertices (VBO)
	uint num_vertices = 0;		// Number of vertices (VBO)
	GLuint VBO = 0;				// Vertex buffer object ID (VBO)

	GLuint* indices = nullptr;	// Array of indices (IBO) 
	uint num_indices = 0;		// Number of indices (IBO)
	GLuint IBO = 0;				// Index buffer object ID (IBO)

	float2* tex_coords = nullptr;	// Array of texture coordinates (tex_coords_id)
	uint num_tex_coords = 0;		// Number of texture coordinates (tex_coords_id
	GLuint tex_coords_id = 0;		// Texture coordinates ID (tex_coords_id)
	GLuint TEX = 0;					// Texture Buffer ID (TEX)

	float3* normals = nullptr;		// Array of normal coordinates
	uint num_normals = 0;			// Number of normal coordinates
};