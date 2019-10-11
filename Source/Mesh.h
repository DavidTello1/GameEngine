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

	void GenVBO(); //vertex buffer object
	void GenIBO(); //index buffer object
	void GenVAO(); //vertex array object

public:
	Vertex* vertices = nullptr;
	uint num_vertices = 0;

	GLuint* indices = nullptr;
	uint num_indices = 0;

	GLuint VBO = 0;
	GLuint IBO = 0;
	GLuint VAO = 0;
};