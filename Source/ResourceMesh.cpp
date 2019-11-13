#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Assimp/include/mesh.h"
#include "par_shapes.h"

#include "mmgr/mmgr.h"

ResourceMesh::ResourceMesh(UID uid) : Resource(uid, Resource::Type::mesh)
{
}

ResourceMesh::~ResourceMesh()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &TEX);

	if (vertices)
	{
		RELEASE_ARRAY(vertices);
		vertices = nullptr;
	}
	if (indices)
	{
		RELEASE_ARRAY(indices);
		indices = nullptr;
	}
	if (tex_coords)
	{
		RELEASE_ARRAY(tex_coords);
		tex_coords = nullptr;
	}
	if (normals)
	{
		RELEASE_ARRAY(normals);
		normals = nullptr;
	}
}

bool ResourceMesh::Import(const char* file, const char* path, const char* buffer)
{
	const aiMesh* mesh;
	LOG("Importing mesh '%s'", (mesh->mName.C_Str()), 'g');

	// Vertices -----------------------
	num_vertices = mesh->mNumVertices;
	vertices = new float3[mesh->mNumVertices];

	if (num_vertices <= 0)
	{
		LOG("Mesh has no vertices", 'e');
		return false;
	}

	LOG("Importing vertices %u", mesh->mNumVertices, 'g');
	for (uint i = 0; i < mesh->mNumVertices; ++i)
	{
		vertices[i].x = mesh->mVertices[i].x;
		vertices[i].y = mesh->mVertices[i].y;
		vertices[i].z = mesh->mVertices[i].z;
	}
	GenVBO();

	// Indices -----------------------
	num_indices = mesh->mNumFaces * 3;
	indices = new GLuint[num_indices];

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3); // assert if face is not a triangle
		memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(GLuint));
	}
	GenIBO();

	// Texture Coordinates -----------------------
	if (mesh->HasTextureCoords(0))
	{
		num_tex_coords = mesh->mNumVertices;
		tex_coords = new float2[num_tex_coords * 2];

		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			tex_coords[i].x = mesh->mTextureCoords[0][i].x;
			tex_coords[i].y = mesh->mTextureCoords[0][i].y;
		}
	}
	GenTexture();

	// Normals -----------------------
	if (mesh->HasNormals())
	{
		LOG("Importing normals %u", mesh->mNumVertices, 'g');
		num_normals = mesh->mNumVertices;
		normals = new float3[num_normals];

		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			normals[i].x = mesh->mNormals[i].x;
			normals[i].y = mesh->mNormals[i].y;
			normals[i].z = mesh->mNormals[i].z;
		}
	}
}

void ResourceMesh::GenVBO()
{
	// Generate VBO
	LOG("Generating VBO", 'v');
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* num_vertices, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenIBO()
{
	// Generate IBO
	LOG("Generating IBO", 'v');
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenTexture()
{
	// Generate Textures
	LOG("Generating Texture", 'v');
	glGenBuffers(1, &tex_coords_id);
	glBindBuffer(GL_ARRAY_BUFFER, tex_coords_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * num_tex_coords, tex_coords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
