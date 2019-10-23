#pragma once

#include "Resources.h"

#include "glew\include\GL\glew.h"


#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"


struct aiMesh;

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

class Mesh : public Resources
{
public:

	Mesh();
	virtual ~Mesh();

	void ImportMesh(aiMesh* mesh);
	void CreateMesh(const shape_type &type, int slices, int stacks, float x = 0, float y = 0, float z = 0, float radius = 0.5);
	void GenTexture();

	void GenVBO(); //vertex buffer object
	void GenIBO(); //index buffer object

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


};