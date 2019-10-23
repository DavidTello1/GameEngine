#include "Mesh.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "Viewport.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "par_shapes.h"


Mesh::Mesh() : Resources(Resources::Type::mesh)
{
}

Mesh::~Mesh()
{
}

void Mesh::ImportMesh(aiMesh* mesh)
{
	LOG("Importing mesh '%s'", (mesh->mName.C_Str()), 'g');

	// Vertices
	num_vertices = mesh->mNumVertices;
	vertices = new float3[num_vertices];

	LOG("Importing vertices %u",mesh->mNumVertices , 'g');
	for (uint i = 0; i < mesh->mNumVertices; ++i)
	{
		vertices[i].x = mesh->mVertices[i].x;
		vertices[i].y = mesh->mVertices[i].y;
		vertices[i].z = mesh->mVertices[i].z;
	}

	// Indices
	num_indices = mesh->mNumFaces * 3;
	indices = new GLuint[num_indices];

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{

		assert(mesh->mFaces[i].mNumIndices == 3); // assert if face is not a triangle

		memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(GLuint));
	}

	//// Normals
	//if (mesh->HasNormals())
	//{
	//	LOG("Importing normals %u",mesh->mNumVertices , 'g');
	//	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	//	{
	//		this->vertices[i].normal[0] = *((GLfloat*)&mesh->mNormals[i].x);
	//		this->vertices[i].normal[1] = *((GLfloat*)&mesh->mNormals[i].y);
	//		this->vertices[i].normal[2] = *((GLfloat*)&mesh->mNormals[i].z);
	//	}
	//}

	// Texture Coordinates
	if (mesh->HasTextureCoords(0))
	{
		num_tex_coords = mesh->mNumVertices;
		tex_coords = new float2[num_tex_coords * 2];

		for (unsigned i = 0; i < mesh->mNumVertices; ++i)
		{
			//LOG("Importing texture coord to vertex %u", i, 'g');
			tex_coords[i].x = mesh->mTextureCoords[0][i].x;
			tex_coords[i].y = mesh->mTextureCoords[0][i].y;
		}
	}
	/*if (new_mesh->HasTextureCoords(0))
	{
		m->num_tex_coords = m->num_vertex;
		m->tex_coords = new float[m->num_tex_coords * 2];

		for (int i = 0; i < m->num_tex_coords; ++i)
		{
			m->tex_coords[i * 2] = new_mesh->mTextureCoords[0][i].x;
			m->tex_coords[(i * 2) + 1] = new_mesh->mTextureCoords[0][i].y;
		}
	}*/

	// Colors
	/*for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		if (mesh->HasVertexColors(i))
		{
			LOG("Importing colors to vertex %u", i , 'g');
			this->vertices[i].color[0] = *((GLfloat*)&mesh->mColors[i]->r);
			this->vertices[i].color[1] = *((GLfloat*)&mesh->mColors[i]->g);
			this->vertices[i].color[2] = *((GLfloat*)&mesh->mColors[i]->b);
			this->vertices[i].color[3] = *((GLfloat*)&mesh->mColors[i]->a);
		}
	}*/

	
	TEX = ModuleResources::checker_texture;

	
	LOG("Generating VBO", 'g');
	GenVBO();
	LOG("Generating IBO", 'g');
	GenIBO();
	LOG("Generating Texture", 'g');
	GenTexture();
}

void Mesh::CreateMesh(const shape_type &type, int slices, int stacks, float x, float y, float z,float radius)
{
	par_shapes_mesh* m;

	if (slices < 3) {
		slices = 3;
		LOG("Slices less than 3, setting to 3",'w');
	}
	if (stacks < 3) {
		stacks = 3;
		LOG("Stacks less than 3, setting to 3",'w');
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
		m = par_shapes_create_torus(slices, stacks,radius);
		break;
	case SPHERE:
		m = par_shapes_create_parametric_sphere(slices,stacks);
		break;
	case BOTTLE:
		m = par_shapes_create_klein_bottle(slices, stacks);
		break;
	case KNOT:
		m = par_shapes_create_trefoil_knot(slices, stacks,radius);
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

	LOG("Creating primitive '%d'", (int)type, 'd');

	// Vertices
	num_vertices = m->npoints;
	vertices = new float3[num_vertices];

	for (uint i = 0; i < num_vertices; ++i)
	{
		int k = i * 3;
		vertices[i].x = m->points[k];
		vertices[i].y = m->points[k+1];
		vertices[i].z = m->points[k+2];
	}

	// Indices
	num_indices = m->ntriangles*3;
	indices = new GLuint[num_indices];

	for (uint i = 0; i < num_indices; ++i)
	{
		indices[i] = (GLuint)m->triangles[i];
	}

	if (m->tcoords != nullptr) {

		// Texture Coordinates
		num_tex_coords = m->npoints;
		tex_coords = new float2[num_tex_coords];

		for (unsigned i = 0; i < num_tex_coords; ++i)
		{
			int k = i * 2;
			tex_coords[i].x = m->tcoords[k];
			tex_coords[i].y = m->tcoords[k+1];
		}
	}
	
	par_shapes_free_mesh(m);

	TEX = ModuleResources::checker_texture;

	LOG("Generating VBO", 'g');
	GenVBO();
	LOG("Generating IBO", 'g');
	GenIBO();
	LOG("Generating Texture", 'g');
	GenTexture();
}

void Mesh::GenTexture()
{
	glGenBuffers(1, &tex_coords_id);
	glBindBuffer(GL_ARRAY_BUFFER, tex_coords_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * num_tex_coords, tex_coords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::GenVBO()
{
	assert(vertices != nullptr);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * num_vertices, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::GenIBO()
{
	assert(indices != nullptr);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
