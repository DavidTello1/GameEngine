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
}

UID ResourceMesh::Import(const aiMesh* ai_mesh, const char* source_file)
{
	ResourceMesh* mesh = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh)); //create new mesh

	// Load Mesh info
	mesh->name = ai_mesh->mName.C_Str();
	LOG("Importing mesh '%s'", (mesh->name.c_str()), 'g');

	bool ret = mesh->LoadMesh(ai_mesh);
	if (!ret)
		LOG("Error Importing info from mesh '%s'", (mesh->name.c_str()), 'e');

	// Saving to own format
	std::string output;
	if (mesh->SaveOwnFormat(output))
	{
		mesh->file = source_file; //get file
		App->file_system->NormalizePath(mesh->file);

		std::string file_name; //get file name
		App->file_system->SplitFilePath(output.c_str(), nullptr, &file_name);
		mesh->exported_file = file_name;

		LOG("Imported aiMesh from [%s] to [%s]", mesh->GetFile(), mesh->GetExportedFile());
	}
	else
	{
		LOG("Importing aiMesh %s FAILED", source_file);
	}

	mesh->UnLoad(); //release memory

	return mesh->uid;
}

bool ResourceMesh::SaveOwnFormat(std::string& output) const
{
	// amount of indices / vertices / normals / texture_coords / AABB
	uint ranges[4] = { num_vertices, num_indices, num_tex_coords, num_normals };
	uint size = sizeof(ranges) + 
		sizeof(uint) * num_indices + 
		sizeof(float3) * num_vertices + 
		sizeof(float3) * num_normals + 
		sizeof(float)* num_tex_coords;

	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	// Store vertices
	cursor += bytes;
	bytes = sizeof(float3) * num_vertices;
	memcpy(cursor, vertices, bytes);

	// Store indices
	cursor += bytes;
	bytes = sizeof(uint) * num_indices;
	memcpy(cursor, indices, bytes);

	// Store tex_coords
	cursor += bytes;
	bytes = sizeof(uint) * num_tex_coords;
	memcpy(cursor, tex_coords, bytes);

	// Store normals
	cursor += bytes;
	bytes = sizeof(float3) * num_normals;
	memcpy(cursor, normals, bytes);

	delete[] data;

	return App->file_system->SaveUnique(output, &data, size, LIBRARY_MESH_FOLDER, "mesh", "dvs_mesh");
}

bool ResourceMesh::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		uint size = App->file_system->Load(LIBRARY_MESH_FOLDER, GetExportedFile(), &buffer);
		char* cursor = buffer;

		// amount of indices / vertices / normals / texture_coords
		uint ranges[4];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		num_vertices = ranges[0];
		num_indices = ranges[1];
		num_tex_coords = ranges[2];
		num_normals = ranges[3];

		// Load vertices
		cursor += bytes;
		bytes = sizeof(float3) * num_vertices;
		vertices = new float3[num_vertices];
		memcpy(vertices, cursor, bytes);
		GenVBO();

		// Load indices
		cursor += bytes;
		bytes = sizeof(uint) * num_indices;
		indices = new uint[num_indices];
		memcpy(indices, cursor, bytes);
		GenIBO();

		// Load tex_coords
		cursor += bytes;
		bytes = sizeof(float2) * num_tex_coords;
		tex_coords = new float2[num_tex_coords];
		memcpy(tex_coords, cursor, bytes);
		GenTexture();
		
		// Load normals
		cursor += bytes;
		bytes = sizeof(float3) * num_normals;
		normals = new float3[num_normals];
		memcpy(normals, cursor, bytes);

		delete[] buffer;
		return true;
	}
	return false;
}

void ResourceMesh::UnLoad()
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


bool ResourceMesh::LoadMesh(const aiMesh* mesh)
{
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

	if (num_indices <= 0)
	{
		LOG("Mesh has no indices", 'e');
		return false;
	}

	LOG("Importing faces %u", mesh->mNumFaces, 'g');
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
	else
	{
		LOG("Mesh has no texture coords", 'e');
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
	else
	{
		LOG("Mesh has no normals", 'e');
	}
	return true;
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
