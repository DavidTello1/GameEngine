#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ComponentMesh.h"

#include "Assimp/include/mesh.h"
#include "par_shapes.h"
#include "SimpleBinStream.h"

#include "mmgr/mmgr.h"

ResourceMesh::ResourceMesh(UID uid) : Resource(uid, Resource::Type::mesh)
{
}

ResourceMesh::~ResourceMesh()
{
	UnLoad();
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
		mesh->original_file = source_file; //get file
		App->file_system->NormalizePath(mesh->original_file);

		std::string file_name = App->file_system->GetFileName(output.c_str());//get exported file
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
	simple::mem_ostream<std::true_type> write_stream; //create output stream

	// Store num of vertices, indices, tex_coords, normals
	write_stream << num_vertices;
	write_stream << num_indices;
	write_stream << num_tex_coords;
	write_stream << num_normals;

	// Store vertices
	for (uint i = 0; i < num_vertices; ++i)
	{
		write_stream << vertices[i].x << vertices[i].y << vertices[i].z;
	}

	// Store indices
	for (uint i = 0; i < num_indices; ++i)
	{
		write_stream << indices[i];
	}

	// Store tex_coords
	for (uint i = 0; i < num_tex_coords; ++i)
	{
		write_stream << tex_coords[i];
	}

	// Store normals
	for (uint i = 0; i < num_normals; ++i)
	{
		write_stream << normals[i];
	}

	const std::vector<char>& data = write_stream.get_internal_vec(); //get vector from stream

	return App->file_system->SaveUnique(output, &data[0], data.size(), LIBRARY_MESH_FOLDER, std::to_string(GetID()).c_str(), "dvs_mesh"); //save
}

bool ResourceMesh::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		uint size = App->file_system->LoadFromPath(LIBRARY_MESH_FOLDER, GetExportedFile(), &buffer);

		simple::mem_istream<std::true_type> read_stream(buffer, size); //create input stream

		// Load num of vertices, indices, tex_coords, normals
		read_stream >> num_vertices;
		read_stream >> num_indices;
		read_stream >> num_tex_coords;
		read_stream >> num_normals;

		// Load vertices
		vertices = new float3[num_vertices];
		for (uint i = 0; i < num_vertices; ++i)
		{
			read_stream >> vertices[i].x >> vertices[i].y >> vertices[i].z;
		}
		GenVBO();

		// Load indices
		indices = new uint[num_indices];
		for (uint i = 0; i < num_indices; ++i)
		{
			read_stream >> indices[i];
		}
		GenIBO();

		// Load tex_coords
		tex_coords = new float2[num_tex_coords];
		for (uint i = 0; i < num_tex_coords; ++i)
		{
			read_stream >> tex_coords[i].x >> tex_coords[i].y;
		}
		GenTexture();
		
		// Load normals
		normals = new float3[num_normals];
		for (uint i = 0; i < num_normals; ++i)
		{
			read_stream >> normals[i].x >> normals[i].y >> normals[i].z;
		}

		delete[] buffer;

		aabb.SetNegativeInfinity();
		aabb.Enclose(vertices, num_vertices);

		return true;
	}
	return false;
}

void ResourceMesh::UnLoad()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

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

void ResourceMesh::CreateShape(ResourceMesh* mesh, const shape_type &type, int slices, int stacks, float x, float y, float z, float radius)
{
	par_shapes_mesh* m;

	if (slices < 3)
	{
		slices = 3;
		LOG("Slices less than 3, setting to 3", 'w');
	}

	switch (type)
	{
	case CYLINDER:
		m = par_shapes_create_cylinder(slices, stacks);
		break;
	case CONE:
		m = par_shapes_create_cone(slices, stacks);
		break;
	case TORUS:
		m = par_shapes_create_torus(slices, stacks, radius);
		break;
	case SPHERE:
		m = par_shapes_create_parametric_sphere(slices, stacks);
		break;
	case BOTTLE:
		m = par_shapes_create_klein_bottle(slices, stacks);
		break;
	case KNOT:
		m = par_shapes_create_trefoil_knot(slices, stacks, radius);
		break;
	case HEMISPHERE:
		m = par_shapes_create_hemisphere(slices, stacks);
		break;
	case PLANE:
		m = par_shapes_create_plane(slices, stacks);
		break;
	case ICOSAHEDRON:
		m = par_shapes_create_icosahedron();
		break;
	case DODECAHEDRON:
		m = par_shapes_create_dodecahedron();
		break;
	case OCTAHEDRON:
		m = par_shapes_create_octahedron();
		break;
	case TETRAHEDRON:
		m = par_shapes_create_tetrahedron();
		break;
	case CUBE:
		m = par_shapes_create_cube();
		break;
	case ROCK:
		m = par_shapes_create_rock(slices*stacks, 2);
		break;

	default:
		break;
	}

	par_shapes_translate(m, x, y, z);

	LOG("Creating primitive '%s'", GetShapeName(type), 'v');

	GameObject* object = App->scene->CreateGameObject(GetShapeName(type), nullptr, true);
	ComponentMesh* mesh_comp = (ComponentMesh*)object->AddComponent(Component::Type::Mesh);
	ComponentMaterial* material = (ComponentMaterial*)object->AddComponent(Component::Type::Material);

	// Vertices ------------------
	mesh->num_vertices = m->npoints;
	mesh->vertices = new float3[mesh->num_vertices];

	for (uint i = 0; i < mesh->num_vertices; ++i)
	{
		int k = i * 3;
		float x = m->points[k];
		float y = m->points[k + 1];
		float z = m->points[k + 2];

		mesh->vertices[i].x = x; 
		mesh->vertices[i].y = y; 
		mesh->vertices[i].z = z; 
	}

	mesh->GenVBO();


	// Indices ---------------------
	mesh->num_indices = m->ntriangles * 3;
	mesh->indices = new GLuint[mesh->num_indices];

	for (uint i = 0; i < mesh->num_indices; ++i)
	{
		mesh->indices[i] = (GLuint)m->triangles[i];
	}

	mesh->GenIBO();

	// Texture -----------------------
	if (m->tcoords != nullptr) {

		mesh->num_tex_coords = m->npoints;
		mesh->tex_coords = new float2[mesh->num_tex_coords];

		for (unsigned i = 0; i < mesh->num_tex_coords; ++i)
		{
			int k = i * 2;
			mesh->tex_coords[i].x = m->tcoords[k];
			mesh->tex_coords[i].y = m->tcoords[k + 1];
		}
	}
	par_shapes_free_mesh(m);

	mesh->GenTexture();

	mesh_comp->SetMesh(mesh);
}
