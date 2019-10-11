#pragma once

#include "Resources.h"

#include "glew\include\GL\glew.h"

struct aiMesh;

struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLubyte color[4];
	GLfloat tex_coords[2];
};

class Mesh : public Resources
{
public:

	Mesh();
	virtual ~Mesh();

	void ImportMesh(aiMesh* mesh);

	void GenVertexBufferObj();
	void GenIndexBufferObj();
	void GenVertexArrayObj();

public:
	Vertex* vertices = nullptr;
	uint vertices_size = 0;

	GLuint* indices = nullptr;
	uint indices_size = 0;

	GLuint vertex_buf_obj = 0;
	GLuint index_buf_obj = 0;
	GLuint vertex_arr_obj = 0;
};