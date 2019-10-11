#include "Mesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

Mesh::Mesh() : Resources(Resources::Type::mesh)
{
}

Mesh::~Mesh()
{
}

void Mesh::ImportMesh(aiMesh* mesh)
{
	// Vertices
	this->vertices_size = mesh->mNumVertices;
	this->vertices = new Vertex[mesh->mNumVertices];

	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		this->vertices[i].position[0] = *((GLfloat*)&mesh->mVertices[i].x);
		this->vertices[i].position[1] = *((GLfloat*)&mesh->mVertices[i].y);
		this->vertices[i].position[2] = *((GLfloat*)&mesh->mVertices[i].z);
	}

	// Normals
	if (mesh->HasNormals())
	{
		for (unsigned i = 0; i < mesh->mNumVertices; ++i)
		{
			this->vertices[i].normal[0] = *((GLfloat*)&mesh->mNormals[i].x);
			this->vertices[i].normal[1] = *((GLfloat*)&mesh->mNormals[i].y);
			this->vertices[i].normal[2] = *((GLfloat*)&mesh->mNormals[i].z);
		}
	}

	// Texture Coordinates
	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		if (mesh->HasTextureCoords(i))
		{
			this->vertices[i].tex_coords[0] = *((GLubyte*)&mesh->mTextureCoords[i]->x);
			this->vertices[i].tex_coords[1] = *((GLubyte*)&mesh->mTextureCoords[i]->y);
		}
	}

	// Colors
	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		if (mesh->HasVertexColors(i))
		{
			this->vertices[i].color[0] = *((GLfloat*)&mesh->mColors[i]->r);
			this->vertices[i].color[1] = *((GLfloat*)&mesh->mColors[i]->g);
			this->vertices[i].color[2] = *((GLfloat*)&mesh->mColors[i]->b);
			this->vertices[i].color[3] = *((GLfloat*)&mesh->mColors[i]->a);
		}
	}

	// Indices
	this->indices_size = mesh->mNumFaces * 3;
	this->indices = new uint[this->indices_size];

	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];

		assert(face.mNumIndices == 3); // assert if face is not a triangle

		this->indices[i * 3] = face.mIndices[0];
		this->indices[i * 3 + 1] = face.mIndices[1];
		this->indices[i * 3 + 2] = face.mIndices[2];
	}
	GenVertexBufferObj();
	GenIndexBufferObj();
	GenVertexArrayObj();
}

void Mesh::GenVertexBufferObj()
{
	assert(vertices != nullptr);

	glGenBuffers(1, &vertex_buf_obj);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf_obj);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::GenIndexBufferObj()
{
	assert(indices != nullptr);

	glGenBuffers(1, &index_buf_obj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf_obj);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices_size, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::GenVertexArrayObj()
{
	glGenVertexArrays(1, &vertex_arr_obj);
	glBindVertexArray(vertex_arr_obj);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf_obj);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position))); //pos
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal))); //normal
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, color))); //color
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tex_coords))); //texture coordinates
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}