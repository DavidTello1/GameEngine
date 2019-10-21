#pragma once

#include "Resources.h"

#include "glew\include\GL\glew.h"
#include "Devil/include/IL/il.h"
#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

struct aiMesh;

//struct Vertex
//{
//	GLfloat position[3];
//	GLfloat normal[3];
//	GLubyte color[4];
//	GLfloat tex_coords[2];
//};

class Mesh : public Resources
{
public:

	Mesh();
	virtual ~Mesh();

	void ImportMesh(aiMesh* mesh);
	void GenTexture();
	void ImportTexture(const char* path);

	void GenVBO(); //vertex buffer object
	void GenIBO(); //index buffer object
	void GenVAO(); //vertex array object

	void GenShaders();

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

	GLuint VAO = 0;					// Vertex Array Object ID (VAO)

	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;
	GLuint shader_program = 0;
};