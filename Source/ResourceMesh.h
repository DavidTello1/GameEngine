#pragma once
#include "Resource.h"
#include "Math.h"
#include <string>

#include "glew\include\GL\glew.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

#define NUM_RANGES 4

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

	static UID Import(const aiMesh* mesh, const char* source_file);
	bool SaveOwnFormat(std::string& output) const;
	bool LoadtoScene();
	void UnLoad();

	//void Draw();

private:
	bool LoadMesh(const aiMesh* mesh);
	void GenVBO();
	void GenIBO();
	void GenTexture();

public:
	float3* vertices = nullptr;	// Array of vertices (VBO)
	uint num_vertices = 0;		// Number of vertices (VBO)
	uint VBO = 0;				// Vertex buffer object ID (VBO)

	uint* indices = nullptr;	// Array of indices (IBO) 
	uint num_indices = 0;		// Number of indices (IBO)
	uint IBO = 0;				// Index buffer object ID (IBO)

	float2* tex_coords = nullptr;	// Array of texture coordinates (tex_coords_id)
	uint num_tex_coords = 0;		// Number of texture coordinates (tex_coords_id)
	uint tex_coords_id = 0;			// Texture coordinates ID (tex_coords_id)
	uint TEX = 0;					// Texture Buffer ID (TEX)

	float3* normals = nullptr;		// Array of normal coordinates
	uint num_normals = 0;			// Number of normal coordinates
};