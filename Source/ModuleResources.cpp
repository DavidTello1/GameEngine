#include "Globals.h"
#include "Application.h"
#include "ModuleSceneBase.h"
#include "ModuleResources.h"
//#include "ModuleFileSystem.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Config.h"
#include <string>

#include "Devil/include/IL/il.h"
#pragma comment (lib, "Devil/lib/x86/DevIL.lib")
#pragma comment (lib, "Devil/lib/x86/ILU.lib")
#pragma comment (lib, "Devil/lib/x86/ILUT.lib")

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "par_shapes.h"

#include "mmgr/mmgr.h"

void AssimpLogCallback(const char *msg, char *userData) 
{
	LOG("%s", msg,'g');
}

ModuleResources::ModuleResources(bool start_enabled) : Module("Resources", start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init(Config* config)
{
	return true;
}

bool ModuleResources::Start(Config* config)
{
	ilInit();

	// Stream log messages to Console and default visual studio debugger
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = AssimpLogCallback;
	aiAttachLogStream(&stream);

	MakeCheckersTexture();
	return true;
}

bool ModuleResources::CleanUp()
{
	return true;
}

//---------------------------------
Component::Type ModuleResources::GetType(const char* path)
{
	char extension[32];
	const char* last_dot = strrchr(path, '.');
	strcpy(extension, last_dot + 1);

	for (int i = 0; i < strlen(extension); i++)
	{
		extension[i] = tolower(extension[i]);
	}

	if (strcmp("obj", extension) == 0 || strcmp("fbx",extension) == 0) // Mesh
	{
		return Component::Type::Mesh;
	}
	else if (strcmp("dds", extension) == 0 || strcmp("png", extension) == 0 || strcmp("jpg", extension) == 0) // Texture
	{
		return Component::Type::Material;
	}

	LOG("File format not supported", 'e');
	return Component::Type::Unknown;
}

void ModuleResources::LoadResource(const char* path, Component::Type type, bool use, uint parent_id)
{
	GameObject* object = nullptr;
	//name
	const char* file_name = strrchr(path, 92) ;
	if (file_name == nullptr) file_name = (strrchr(path, '/') != nullptr) ? strrchr(path, '/') : "GameObject";
	file_name++;

	if (type == Component::Type::Unknown)
		type = GetType(path);

	if (type == Component::Type::Mesh) // Mesh
	{
		LOG("Mesh resource type",'g');
		const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene != nullptr && scene->HasMeshes())
		{
			LOG("Number of meshes: %u", scene->mNumMeshes, 'g');

			for (uint i = 0; i < scene->mNumMeshes; ++i)
			{	
				if (use)
				{
					object = App->scene->CreateGameObj(file_name,parent_id);
					ComponentMesh* mesh_component = (ComponentMesh*)object->AddComponent(Component::Type::Mesh);
					aiMesh* mesh = scene->mMeshes[i];

					ImportMesh(mesh, mesh_component);
					object->AddComponent(Component::Type::Material);
					object->AddComponent(Component::Type::Renderer);
				}
			}
			aiReleaseImport(scene);
		}
		else
			LOG("Error loading mesh %s", path, 'e');

	}
	else if (type == Component::Type::Material) // Texture
	{
		if (!App->scene->selected_go.empty())
		{
			// Devil
			uint imageID;
			ilGenImages(1, &imageID);
			ilBindImage(imageID);
			ilEnable(IL_ORIGIN_SET);
			ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

			bool loaded = ilLoadImage(path);
			if (!loaded) LOG("IMAGE '%s' COULD NOT BE LOADED PROPERLY", path, 'e');

			LogImageInfo();

			GLuint tex = ImportTexture(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
			ilDeleteImages(1, &imageID);

			if (use)
			{
				for (GameObject* object : App->scene->selected_go)
				{
					bool has_mesh = false;
					// Check if has a mesh component (to be replaced by boolean)
					for (uint i = 0; i < object->components.size(); i++)
					{
						if (object->components[i]->GetType() == Component::Type::Mesh)
						{
							ComponentMesh* mesh = (ComponentMesh*)object->components[i];
							mesh->TEX = tex;
							has_mesh = true;
							LOG("Texture %s applied to object %s %u", file_name, object->GetName(), object->GetUID(),'d');
							break;
						}
					}
					if (!has_mesh)
					{
						LOG("Object '%s %u' is missing 'Mesh' component, could not apply texture",object->GetName(),object->GetUID(),'e');
					}
				}
				
			}
		}
		else
		{
			LOG("Cannot load texture without GameObject", 'e');
		}
	}

	
}

void ModuleResources::ImportMesh(aiMesh* mesh, ComponentMesh* mesh_component)
{
	LOG("Importing mesh '%s'", (mesh->mName.C_Str()), 'g');

	// Vertices -----------------------
	mesh_component->num_vertices = mesh->mNumVertices;
	mesh_component->vertices = new float3[mesh->mNumVertices];

	if (mesh_component->num_vertices > 0)
	{
		mesh_component->min_vertex.x = mesh->mVertices[0].x;
		mesh_component->min_vertex.y = mesh->mVertices[0].y;
		mesh_component->min_vertex.z = mesh->mVertices[0].z;

		mesh_component->max_vertex.x = mesh->mVertices[0].x;
		mesh_component->max_vertex.y = mesh->mVertices[0].y;
		mesh_component->max_vertex.z = mesh->mVertices[0].z;
	}
	else {
		LOG("Mesh has no vertices", 'e');
		return;
	}

	LOG("Importing vertices %u", mesh->mNumVertices, 'g');
	for (uint i = 0; i < mesh->mNumVertices; ++i)
	{
		float x = mesh->mVertices[i].x;
		float y = mesh->mVertices[i].y;
		float z = mesh->mVertices[i].z;

		mesh_component->vertices[i].x = x;
		mesh_component->vertices[i].y = y;
		mesh_component->vertices[i].z = z;

		// Bounding box setting up
		if (x < mesh_component->min_vertex.x) mesh_component->min_vertex.x = x;
		if (y < mesh_component->min_vertex.y) mesh_component->min_vertex.y = y;
		if (z < mesh_component->min_vertex.z) mesh_component->min_vertex.z = z;

		if (x > mesh_component->max_vertex.x) mesh_component->max_vertex.x = x;
		if (y > mesh_component->max_vertex.y) mesh_component->max_vertex.y = y;
		if (z > mesh_component->max_vertex.z) mesh_component->max_vertex.z = z;
		
	}

	GenVBO(mesh_component);


	// Indices -----------------------
	mesh_component->num_indices = mesh->mNumFaces * 3;
	mesh_component->indices = new GLuint[mesh_component->num_indices];

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3); // assert if face is not a triangle
		memcpy(&mesh_component->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(GLuint));
	}

	GenIBO(mesh_component);

	// Texture Coordinates -----------------------
	if (mesh->HasTextureCoords(0))
	{
		mesh_component->num_tex_coords = mesh->mNumVertices;
		mesh_component->tex_coords = new float2[mesh_component->num_tex_coords * 2];

		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			mesh_component->tex_coords[i].x = mesh->mTextureCoords[0][i].x;
			mesh_component->tex_coords[i].y = mesh->mTextureCoords[0][i].y;
		}
	}

	GenTexture(mesh_component);

	GenBoundingBox(mesh_component);

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
}

void ModuleResources::GenBoundingBox(ComponentMesh * mesh_component)
{
	//Bounding box
	float3 min = mesh_component->min_vertex;
	float3 max = mesh_component->max_vertex;

	mesh_component->bounding_box[0] = { min.x,min.y,min.z };
	mesh_component->bounding_box[1] = { min.x,min.y,max.z };
	mesh_component->bounding_box[2] = { max.x,min.y,max.z };
	mesh_component->bounding_box[3] = { max.x,min.y,min.z };

	mesh_component->bounding_box[4] = { min.x,max.y,min.z };
	mesh_component->bounding_box[5] = { min.x,max.y,max.z };
	mesh_component->bounding_box[6] = { max.x,max.y,max.z };
	mesh_component->bounding_box[7] = { max.x,max.y,min.z };

	float3 c = (mesh_component->min_vertex + mesh_component->max_vertex) / 2;
	mesh_component->center = c;
	mesh_component->bounding_box[8] = mesh_component->center;

	mesh_component->bounding_box[9]  = { c.x,c.y,min.z };  //Face 0437 center
	mesh_component->bounding_box[10]  = { min.x,c.y,c.z };  //Face 0415 center
	mesh_component->bounding_box[11] = { max.x,c.y,c.z }; //Face 3726 center
	mesh_component->bounding_box[12] = { c.x,c.y,max.z }; //Face 1256 center
	//mesh_component->bounding_box[13] = { 0,0,0 }; // camera
	//mesh_component->bounding_box[11] = { c.x,max.y,c.z }; //Face 4567 center (top face)
	//mesh_component->bounding_box[14] = { c.x,min.y,c.z }; //Face 0123 center (bot face)



	mesh_component->size = mesh_component->max_vertex - mesh_component->min_vertex;

	// VBO
	glGenBuffers(1, &mesh_component->bb_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_component->bb_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_component->bounding_box), mesh_component->bounding_box, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//IBO
	glGenBuffers(1, &mesh_component->bb_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_component->bb_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(App->resources->bbox_indices), App->resources->bbox_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	LOG("BoundingBox vertices", 'd');

	for (int i = 0; i < 8; i++)
	{
		LOG("{ %f , %f , %f }", mesh_component->bounding_box[i].x, mesh_component->bounding_box[i].y, mesh_component->bounding_box[i].z, 'd');
	}
}

GLuint ModuleResources::ImportTexture(int width, int height,int internal_format, int format, unsigned char* image)
{
	//LOG("Importing texture [%d,%d] data size: %u", width, height, sizeof(image)*sizeof(unsigned char), 'g');
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

void ModuleResources::UnLoadResource()
{

}

void ModuleResources::CreateShape(const shape_type &type, int slices, int stacks, float x, float y, float z, float radius, uint parent_id)
{
	par_shapes_mesh* m;

	if (slices < 3)
	{
		slices = 3;
		LOG("Slices less than 3, setting to 3", 'w');
	}
	if (stacks < 3)
	{
		stacks = 3;
		LOG("Stacks less than 3, setting to 3", 'w');
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

	GameObject* object = App->scene->CreateGameObj(GetShapeName(type), parent_id, true);
	ComponentMesh* mesh = (ComponentMesh*)object->AddComponent(Component::Type::Mesh);
	object->AddComponent(Component::Type::Material);

	// Vertices ------------------
	mesh->num_vertices = m->npoints;
	mesh->vertices = new float3[mesh->num_vertices];
	if (mesh->num_vertices >= 3)
	{
		mesh->min_vertex.x = m->points[0];
		mesh->min_vertex.y = m->points[1];
		mesh->min_vertex.z = m->points[2];
		mesh->max_vertex.x = m->points[0];
		mesh->max_vertex.y = m->points[1];
		mesh->max_vertex.z = m->points[2];
	}
	else {
		LOG("Mesh has no vertices", 'e');
		return;
	}
	for (uint i = 0; i < mesh->num_vertices; ++i)
	{
		int k = i * 3;
		float x = m->points[k];
		float y = m->points[k + 1];
		float z = m->points[k + 2];

		mesh->vertices[i].x = x; 
		mesh->vertices[i].y = y; 
		mesh->vertices[i].z = z; 

		// Bounding box setting up
		if (x < mesh->min_vertex.x) mesh->min_vertex.x = x;
		if (y < mesh->min_vertex.y) mesh->min_vertex.y = y;
		if (z < mesh->min_vertex.z) mesh->min_vertex.z = z;

		if (x > mesh->max_vertex.x) mesh->max_vertex.x = x;
		if (y > mesh->max_vertex.y) mesh->max_vertex.y = y;
		if (z > mesh->max_vertex.z) mesh->max_vertex.z = z;
	}

	GenVBO(mesh);


	// Indices ---------------------
	mesh->num_indices = m->ntriangles * 3;
	mesh->indices = new GLuint[mesh->num_indices];

	for (uint i = 0; i < mesh->num_indices; ++i)
	{
		mesh->indices[i] = (GLuint)m->triangles[i];
	}

	GenIBO(mesh);

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

	GenTexture(mesh);

	//if (type < 8) //idk but has to be like this for now
		//GenBoundingBox(mesh);



}
void ModuleResources::GenVBO(ComponentMesh * mesh_component)
{
	// Generate VBO
	LOG("Generating VBO", 'g');
	glGenBuffers(1, &mesh_component->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_component->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* mesh_component->num_vertices, mesh_component->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleResources::GenIBO(ComponentMesh * mesh_component)
{
	// Generate IBO
	LOG("Generating IBO", 'g');
	glGenBuffers(1, &mesh_component->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_component->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh_component->num_indices, mesh_component->indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleResources::GenTexture(ComponentMesh * mesh_component)
{
	// Generate Textures
	LOG("Generating Texture", 'g');
	glGenBuffers(1, &mesh_component->tex_coords_id);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_component->tex_coords_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * mesh_component->num_tex_coords, mesh_component->tex_coords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ModuleResources::MakeCheckersTexture()
{
	const int checkImageWidth = 256;
	const int checkImageHeight = 256;
	static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

	int c;
	for (int i = 0; i < checkImageHeight; i++)
	{
		for (int j = 0; j < checkImageWidth; j++)
		{
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &checker_texture);
	glBindTexture(GL_TEXTURE_2D, checker_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);
}

void ModuleResources::LogImageInfo()
{
	char* s;
	char* c;
	switch (ilGetInteger(IL_IMAGE_FORMAT)) {
	case IL_COLOR_INDEX: s = "IL_COLOR_INDEX"; break;
	case IL_ALPHA: s = "IL_ALPHA"; break;
	case IL_RGB: s = "IL_RGB"; break;
	case IL_RGBA: s = "IL_RGBA"; break;
	case IL_BGR: s = "IL_BGR"; break;
	case IL_BGRA: s = "IL_BGRA"; break;
	case IL_LUMINANCE: s = "IL_LUMINANCE"; break;
	case  IL_LUMINANCE_ALPHA: s = "IL_LUMINANCE_ALPHA"; break;
	}
	switch (ilGetInteger(IL_IMAGE_TYPE)) {
	case IL_BYTE: c = "IL_BYTE"; break;
	case IL_UNSIGNED_BYTE: c = "IL_UNSIGNED_BYTE"; break;
	case IL_SHORT: c = "IL_SHORT"; break;
	case IL_UNSIGNED_SHORT: c = "IL_UNSIGNED_SHORT"; break;
	case IL_INT: c = "IL_INT"; break;
	case IL_UNSIGNED_INT: c = "IL_UNSIGNED_INT"; break;
	case IL_FLOAT: c = "IL_FLOAT"; break;
	case IL_DOUBLE: c = "IL_DOUBLE"; break;
	case IL_HALF: c = "IL_HALF"; break;
	}

	LOG("Width: %d, Height %d, Bytes per Pixel %d",
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT),
		ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL), 'g');
	LOG("Original image format: %s, data type: %s", s, c, 'g');
}