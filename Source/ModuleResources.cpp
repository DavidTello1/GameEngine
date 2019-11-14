#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "Config.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Devil/include/IL/il.h"
#pragma comment (lib, "Devil/lib/x86/DevIL.lib")
#pragma comment (lib, "Devil/lib/x86/ILU.lib")
#pragma comment (lib, "Devil/lib/x86/ILUT.lib")

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
	LOG("Loading DevIL", 'd');
	ilInit();

	// Stream log messages to Console and default visual studio debugger
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = AssimpLogCallback;
	aiAttachLogStream(&stream);

	//MakeCheckersTexture();
	return true;
}

bool ModuleResources::CleanUp()
{
	return true;
}

bool ModuleResources::ImportResource(const char* path, UID uid)
{
	std::string final_path;
	App->file_system->SplitFilePath(path, nullptr, &final_path);
	final_path = ASSETS_FOLDER + final_path;

	if (App->file_system->CopyFromOutsideFS(path, final_path.c_str()) == true) //copy file to final_path
	{
		// Get Extension
		std::string extension;
		App->file_system->SplitFilePath(path, nullptr, nullptr, &extension);

		// Get Type
		Resource::Type type;
		if (strcmp("obj", extension.c_str()) == 0 || strcmp("fbx", extension.c_str()) == 0)
		{
			type = Resource::Type::model;
			LOG("Importing resource model from %s", path, 'd');
		}
		else if (strcmp("dds", extension.c_str()) == 0 || strcmp("png", extension.c_str()) == 0 || strcmp("jpg", extension.c_str()) == 0)
		{
			type = Resource::Type::material;
			LOG("Importing resource material from %s", path, 'd');
		}
		else
		{
			type = Resource::Type::unknown;
			LOG("File format not supported from %s", path, 'e');
			return false;
		}

		// Check if file has already been loaded
		std::string file = final_path;
		App->file_system->NormalizePath(file);
		for (std::map<UID, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
		{
			if (it->second->file.compare(file) == 0)
			{
				uid = it->first;
				LOG("File is already loaded in memory", 'd');
				return true;
			}
		}

		// Import file
		bool import_ok = false;
		std::string written_file;

		switch (type)
		{
		case Resource::model:
			import_ok = ResourceModel::Import(final_path, written_file);
			break;
		case Resource::material:
			import_ok = ResourceMaterial::Import(final_path, written_file);
			break;
		}

		// Check if import went ok
		if (import_ok == true)
		{
			// Create new Resource
			Resource* res = CreateResource(type);
			res->file = final_path;
			App->file_system->NormalizePath(res->file);
			uid = res->uid;

			// Exported file of Resource
			std::string exported_file;
			App->file_system->SplitFilePath(written_file.c_str(), nullptr, &exported_file);
			res->exported_file = exported_file.c_str();
			//LOG("Imported successful from [%s] to [%s]", res->GetFile(), res->GetExportedFile());

			// Name of Resource
			std::string name;
			App->file_system->SplitFilePath(final_path.c_str(), nullptr, &name);
			App->file_system->SplitFilePath(res->file.c_str(), nullptr, &res->name);

			res->name = name;
			if (res->name.empty())
				res->name = res->exported_file;

			size_t pos_dot = res->name.find_last_of(".");
			if (pos_dot != std::string::npos)
				res->name.erase(res->name.begin() + pos_dot, res->name.end());
		}
		else
		{
			LOG("Importing of [%s] FAILED", final_path);
			return false;
		}
		return true;
	}
	return false;

	//// name
	//const char* file_name = strrchr(path, 92) ;
	//if (file_name == nullptr) file_name = (strrchr(path, '/') != nullptr) ? strrchr(path, '/') : "GameObject";
	//file_name++;

	////type
	//char extension[32];
	//const char* last_dot = strrchr(path, '.');
	//strcpy_s(extension, last_dot + 1);

	//for (uint i = 0; i < strlen(extension); i++)
	//{
	//	extension[i] = tolower(extension[i]);
	//}



	//else if (type == Component::Type::Material) // Texture
	//{
	//	ComponentMaterial* material_loaded = nullptr;
	//	GLuint tex;

	//	if (App->scene->IsMaterialLoaded(path)) //if material is already loaded
	//		material_loaded = App->scene->GetMaterial(path);
	//	else
	//	{
	//		// Devil
	//		uint imageID;
	//		ilGenImages(1, &imageID);
	//		ilBindImage(imageID);
	//		ilEnable(IL_ORIGIN_SET);
	//		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	//		bool loaded = ilLoadImage(path);
	//		if (!loaded) LOG("IMAGE '%s' COULD NOT BE LOADED PROPERLY", path, 'e');

	//		LogImageInfo();

	//		tex = ImportTexture(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
	//		ilDeleteImages(1, &imageID);
	//		LOG("Texture %s loaded", file_name, 'd');
	//	}

	//	if (use)
	//	{
	//		for (GameObject* object : App->scene->gameObjects)
	//		{
	//			if (!object->is_selected) continue;

	//			if (object->HasComponent(Component::Type::Mesh))
	//			{
	//				ComponentMesh* mesh = (ComponentMesh*)object->GetComponent(Component::Type::Mesh);
	//				if (App->scene->IsMaterialLoaded(path)) //if material is already loaded
	//				{
	//					material_loaded = App->scene->GetMaterial(path);
	//					mesh->TEX = material_loaded->tex_id;
	//				}
	//				else
	//					mesh->TEX = tex;
	//			}

	//			//if (object->HasChilds()) //if object is parent
	//			//{
	//				if (!object->HasComponent(Component::Type::Material)) //if object has not got material add one
	//					object->AddComponent(Component::Type::Material);

	//				ComponentMaterial* material = (ComponentMaterial*)object->GetComponent(Component::Type::Material);
	//				if (App->scene->IsMaterialLoaded(path)) //if material is already loaded
	//				{
	//					material_loaded = App->scene->GetMaterial(path);
	//					strcpy_s(material->path, 256, material_loaded->path);
	//					material->tex_id = material_loaded->tex_id;
	//					material->width = material_loaded->width;
	//					material->height = material_loaded->height;
	//				}
	//				else
	//				{
	//					strcpy_s(material->path, 256, path);
	//					material->tex_id = tex;
	//					material->width = tex_width;
	//					material->height = tex_height;

	//					App->scene->materials.push_back(material);

	//					LOG("Texture %s applied to object %s %u", file_name, object->GetName(), object->GetUID(), 'd');
	//				}
	//			//}
	//		}
	//	}
	//	
	//}
}

Resource* ModuleResources::CreateResource(Resource::Type type, UID force_uid)
{
	Resource* ret = nullptr;
	UID uid;

	if (force_uid != 0 && GetResource(force_uid) == nullptr)
		uid = force_uid;
	else
		uid = GenerateUID();

	switch (type)
	{
	case Resource::model:
		ret = new ResourceModel(uid);
		break;
	case Resource::mesh:
		ret = (Resource*) new ResourceMesh(uid);
		break;
	case Resource::material:
		ret = (Resource*) new ResourceMaterial(uid);
		break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
	}

	return ret;
}


//GLuint ModuleResources::ImportTexture(int width, int height,int internal_format, int format, unsigned char* image)
//{
//	//LOG("Importing texture [%d,%d] data size: %u", width, height, sizeof(image)*sizeof(unsigned char), 'g');
//	GLuint texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	tex_height = height;
//	tex_width = width;
//	return texture;
//}

//void ModuleResources::CreateShape(const shape_type &type, int slices, int stacks, float x, float y, float z, float radius, GameObject* parent)
//{
//	par_shapes_mesh* m;
//
//	if (slices < 3)
//	{
//		slices = 3;
//		LOG("Slices less than 3, setting to 3", 'w');
//	}
//	if (stacks < 3)
//	{
//		stacks = 3;
//		LOG("Stacks less than 3, setting to 3", 'w');
//	}
//
//	switch (type)
//	{
//	case CYLINDER:
//		m = par_shapes_create_cylinder(slices, stacks);
//		break;
//	case CONE:
//		m = par_shapes_create_cone(slices, stacks);
//		break;
//	case TORUS:
//		m = par_shapes_create_torus(slices, stacks, radius);
//		break;
//	case SPHERE:
//		m = par_shapes_create_parametric_sphere(slices, stacks);
//		break;
//	case BOTTLE:
//		m = par_shapes_create_klein_bottle(slices, stacks);
//		break;
//	case KNOT:
//		m = par_shapes_create_trefoil_knot(slices, stacks, radius);
//		break;
//	case HEMISPHERE:
//		m = par_shapes_create_hemisphere(slices, stacks);
//		break;
//	case PLANE:
//		m = par_shapes_create_plane(slices, stacks);
//		break;
//	case ICOSAHEDRON:
//		m = par_shapes_create_icosahedron();
//		break;
//	case DODECAHEDRON:
//		m = par_shapes_create_dodecahedron();
//		break;
//	case OCTAHEDRON:
//		m = par_shapes_create_octahedron();
//		break;
//	case TETRAHEDRON:
//		m = par_shapes_create_tetrahedron();
//		break;
//	case CUBE:
//		m = par_shapes_create_cube();
//		break;
//	case ROCK:
//		m = par_shapes_create_rock(slices*stacks, 2);
//		break;
//
//	default:
//		break;
//	}
//
//	par_shapes_translate(m, x, y, z);
//
//	LOG("Creating primitive '%s'", GetShapeName(type), 'v');
//
//	GameObject* object = App->scene->CreateGameObject(GetShapeName(type), parent, true);
//	ComponentMesh* mesh = (ComponentMesh*)object->AddComponent(Component::Type::Mesh);
//	object->AddComponent(Component::Type::Material);
//
//	// Vertices ------------------
//	mesh->num_vertices = m->npoints;
//	mesh->vertices = new float3[mesh->num_vertices];
//	if (mesh->num_vertices >= 3)
//	{
//		object->min_vertex.x = m->points[0];
//		object->min_vertex.y = m->points[1];
//		object->min_vertex.z = m->points[2];
//		object->max_vertex.x = m->points[0];
//		object->max_vertex.y = m->points[1];
//		object->max_vertex.z = m->points[2];
//	}
//	else {
//		LOG("Mesh has no vertices", 'e');
//		return;
//	}
//	for (uint i = 0; i < mesh->num_vertices; ++i)
//	{
//		int k = i * 3;
//		float x = m->points[k];
//		float y = m->points[k + 1];
//		float z = m->points[k + 2];
//
//		mesh->vertices[i].x = x; 
//		mesh->vertices[i].y = y; 
//		mesh->vertices[i].z = z; 
//
//		// Bounding box setting up
//		if (x < object->min_vertex.x) object->min_vertex.x = x;
//		if (y < object->min_vertex.y) object->min_vertex.y = y;
//		if (z < object->min_vertex.z) object->min_vertex.z = z;
//
//		if (x > object->max_vertex.x) object->max_vertex.x = x;
//		if (y > object->max_vertex.y) object->max_vertex.y = y;
//		if (z > object->max_vertex.z) object->max_vertex.z = z;
//	}
//
//	GenVBO(mesh);
//
//
//	// Indices ---------------------
//	mesh->num_indices = m->ntriangles * 3;
//	mesh->indices = new GLuint[mesh->num_indices];
//
//	for (uint i = 0; i < mesh->num_indices; ++i)
//	{
//		mesh->indices[i] = (GLuint)m->triangles[i];
//	}
//
//	GenIBO(mesh);
//
//	// Texture -----------------------
//	if (m->tcoords != nullptr) {
//
//		mesh->num_tex_coords = m->npoints;
//		mesh->tex_coords = new float2[mesh->num_tex_coords];
//
//		for (unsigned i = 0; i < mesh->num_tex_coords; ++i)
//		{
//			int k = i * 2;
//			mesh->tex_coords[i].x = m->tcoords[k];
//			mesh->tex_coords[i].y = m->tcoords[k + 1];
//		}
//	}
//	par_shapes_free_mesh(m);
//
//	GenTexture(mesh);
//
//	if (type < 8) //idk but has to be like this for now, otherwise ImGui crashes
//	{
//		object->GenBoundingBox();
//		object->is_valid_dimensions = true;
//	}
//}
//
//void ModuleResources::MakeCheckersTexture()
//{
//	const int checkImageWidth = 256;
//	const int checkImageHeight = 256;
//	static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
//
//	int c;
//	for (int i = 0; i < checkImageHeight; i++)
//	{
//		for (int j = 0; j < checkImageWidth; j++)
//		{
//			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
//			checkImage[i][j][0] = (GLubyte)c;
//			checkImage[i][j][1] = (GLubyte)c;
//			checkImage[i][j][2] = (GLubyte)c;
//			checkImage[i][j][3] = (GLubyte)255;
//		}
//	}
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//	glGenTextures(1, &checker_texture);
//	glBindTexture(GL_TEXTURE_2D, checker_texture);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
//		GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//		GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
//		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
//		checkImage);
//}
//
//void ModuleResources::LogImageInfo()
//{
//	char* s;
//	char* c;
//	switch (ilGetInteger(IL_IMAGE_FORMAT)) {
//	case IL_COLOR_INDEX: s = "IL_COLOR_INDEX"; break;
//	case IL_ALPHA: s = "IL_ALPHA"; break;
//	case IL_RGB: s = "IL_RGB"; break;
//	case IL_RGBA: s = "IL_RGBA"; break;
//	case IL_BGR: s = "IL_BGR"; break;
//	case IL_BGRA: s = "IL_BGRA"; break;
//	case IL_LUMINANCE: s = "IL_LUMINANCE"; break;
//	case  IL_LUMINANCE_ALPHA: s = "IL_LUMINANCE_ALPHA"; break;
//	}
//	switch (ilGetInteger(IL_IMAGE_TYPE)) {
//	case IL_BYTE: c = "IL_BYTE"; break;
//	case IL_UNSIGNED_BYTE: c = "IL_UNSIGNED_BYTE"; break;
//	case IL_SHORT: c = "IL_SHORT"; break;
//	case IL_UNSIGNED_SHORT: c = "IL_UNSIGNED_SHORT"; break;
//	case IL_INT: c = "IL_INT"; break;
//	case IL_UNSIGNED_INT: c = "IL_UNSIGNED_INT"; break;
//	case IL_FLOAT: c = "IL_FLOAT"; break;
//	case IL_DOUBLE: c = "IL_DOUBLE"; break;
//	case IL_HALF: c = "IL_HALF"; break;
//	}
//
//	LOG("Width: %d, Height %d, Bytes per Pixel %d",
//		ilGetInteger(IL_IMAGE_WIDTH),
//		ilGetInteger(IL_IMAGE_HEIGHT),
//		ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL), 'g');
//	LOG("Original image format: %s, data type: %s", s, c, 'g');
//}