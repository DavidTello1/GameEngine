#pragma once
#include "Component.h"
#include "Math.h"

#include "glew\include\GL\glew.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* obj);
	virtual ~ComponentMesh();

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

public:

	float3 min_vertex = { 0,0,0 };
	float3 max_vertex = { 0,0,0 };

	float3 bounding_box[8];
	GLuint bb_VBO = 0;
	GLuint bb_IBO = 0;
	

};

