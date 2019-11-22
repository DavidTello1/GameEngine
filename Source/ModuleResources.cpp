#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "Assets.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "Config.h"
#include "PathNode.h"

#include "Assimp/include/cimport.h"

#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"

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
	LOG("Loading DevIL", 'd');
	ilInit();

	// Stream log messages to Console and default visual studio debugger
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = AssimpLogCallback;
	aiAttachLogStream(&stream);
	return true;
}

bool ModuleResources::Start(Config* config)
{
	//MakeCheckersTexture();
	//LoadResourcesData();

	//UpdateAssets();
	LoadAssetsIcons();

	return true;
}

bool ModuleResources::CleanUp()
{
	LOG("Unloading Resource Manager");

	//SaveResources();

	for (std::map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);

	for (std::vector<Resource*>::iterator it = removed.begin(); it != removed.end(); ++it)
		RELEASE(*it);

	resources.clear();

	return true;
}

bool ModuleResources::ImportFromOutside(const char* path, UID uid)
{
	std::string final_path = App->file_system->GetFileName(path); //get file name
	final_path = ASSETS_FOLDER + final_path;

	App->file_system->NormalizePath(final_path);

	if (App->file_system->CopyFromOutsideFS(path, final_path.c_str()) == true) //copy file to final_path
	{
		return ImportResource(final_path.c_str(), uid);
	}
	return false;
}

bool ModuleResources::ImportResource(const char* final_path, UID uid)
{
	bool import_ok = false;
	std::string written_file;
	Resource::Type type = GetResourceType(final_path); //get resource type

	//if (CheckLoaded(final_path, uid) == true) // Check if file has already been loaded and if so, init uid
	//{
	//	Resource* res = CreateInitResource(type, uid, final_path, written_file); //create and init resource
	//	LOG("File is already loaded in memory", 'd');
	//	return true;
	//}


	switch (type) //import depending on type
	{
	case Resource::model:
	{
		import_ok = ResourceModel::Import(final_path, written_file);
		break;
	}
	case Resource::material:
		import_ok = ResourceMaterial::Import(final_path);
		break;
	}

	if (import_ok == true)
	{
		Resource* res = CreateInitResource(type, uid, final_path, written_file); //create and init resource
	}
	else
	{
		LOG("Importing of [%s] FAILED", final_path);
		return false;
	}
	return true;
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
	case Resource::unknown:
		break;
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

Resource* ModuleResources::CreateInitResource(Resource::Type type, UID uid, const char* final_path, std::string& written_file)
{
	Resource* res = CreateResource(type); //create new resource
	uid = res->uid; //fill uid

	res->original_file = final_path; //get file
	std::string exported_file = App->file_system->GetFileName(written_file.c_str()); //get exported file

	if (exported_file.c_str() != NULL) //check for errors
	{
		res->exported_file = exported_file.c_str();
		LOG("Imported successful from [%s] to [%s]", res->GetFile(), res->GetExportedFile(), 'd');
	}
	else
	{
		LOG("Unable to export file [%s]", res->GetFile(), 'e');
		return false;
	}

	res->name = App->file_system->GetFileName(res->original_file.c_str()); //fill resource name

	if (res->name.empty()) //if empty, name = exported file
		res->name = res->exported_file;

	size_t pos_dot = res->name.find_last_of("."); //get last dot position
	if (pos_dot != std::string::npos)
		res->name.erase(res->name.begin() + pos_dot, res->name.end()); //erase extension from name

	return res;
}

void ModuleResources::RemoveResource(UID uid)
{
	std::map<UID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		App->file_system->Remove(it->second->GetExportedFile());

		char tmp[256];
		sprintf_s(tmp, 255, "%s%s", GetDirectory(it->second->GetType()), it->second->GetExportedFile());
		App->file_system->Remove(tmp);

		removed.push_back(it->second);

		resources.erase(it);
	}
}

const Resource * ModuleResources::GetResource(UID uid) const
{
	if (resources.find(uid) != resources.end())
		return resources.at(uid);
	return nullptr;
}

Resource * ModuleResources::GetResource(UID uid)
{
	std::map<UID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

UID ModuleResources::GenerateUID()
{
	++last_uid;
	SaveUID();
	return last_uid;
}

void ModuleResources::LoadUID()
{
	std::string file(SETTINGS_FOLDER);
	file += "Last UID";

	char *buf = nullptr;
	uint size = App->file_system->Load(file.c_str(), &buf);

	if (size == sizeof(last_uid))
	{
		last_uid = *((UID*)buf);
		RELEASE_ARRAY(buf);
	}
	else
	{
		LOG("WARNING! Cannot read resource UID from file [%s] - Generating a new one", file.c_str());
		SaveUID();
	}
}

void ModuleResources::SaveUID() const
{
	std::string file(SETTINGS_FOLDER);
	file += "Last UID";

	uint size = App->file_system->Save(file.c_str(), (const char*)&last_uid, sizeof(last_uid));

	if (size != sizeof(last_uid))
		LOG("WARNING! Cannot write resource UID into file [%s]", file.c_str());
}

const char* ModuleResources::GetDirectory(Resource::Type type) const
{
	static_assert(Resource::Type::unknown == 0, "String list needs update");

	static const char* dirs_by_type[] = {
		LIBRARY_MODEL_FOLDER, LIBRARY_MATERIAL_FOLDER,
		LIBRARY_MESH_FOLDER, LIBRARY_SCENE_FOLDER
	};

	return dirs_by_type[type];
}

Resource::Type ModuleResources::GetResourceType(const char* path) const
{
	Resource::Type type;
	std::string extension = App->file_system->GetExtension(path);
	App->file_system->ToLower(extension);

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
	}

	return type;
}

bool ModuleResources::CheckLoaded(std::string path, UID uid)
{
	for (std::map<UID, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->original_file.compare(path) == 0)
		{
			uid = it->first;
			return true;
		}
	}
	return false;
}

void ModuleResources::UpdateAssets()
{
	std::vector<std::string> ignore_extensions;
	ignore_extensions.push_back("meta");
	PathNode assets = App->file_system->GetAllFiles("Assets", nullptr, &ignore_extensions);
	UpdateAssetsFolder(assets);
}

void ModuleResources::UpdateAssetsFolder(const PathNode& node)
{
	//If node is a file
	if (node.file == true)
	{
		ImportResource(node.path.c_str());
	}

	//If node folder has something inside
	else if (node.leaf == false)
	{
		for (uint i = 0; i < node.children.size(); i++)
		{
			UpdateAssetsFolder(node.children[i]);
		}
	}
}

void ModuleResources::LoadAssetsIcons()
{

	ResourceMaterial* resource = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	App->editor->tab_assets->folder_icon = resource->LoadTexture("Settings/Icons/folder_icon.png");

	resource = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	App->editor->tab_assets->file_icon = resource->LoadTexture("Settings/Icons/file_icon.png");

	resource = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	App->editor->tab_assets->model_icon = resource->LoadTexture("Settings/Icons/model_icon.png");

	resource = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	App->editor->tab_assets->material_icon = resource->LoadTexture("Settings/Icons/material_icon.png");

	resource = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	App->editor->tab_assets->scene_icon = resource->LoadTexture("Settings/Icons/scene_icon.png");

	glBindTexture(GL_TEXTURE_2D, 0); //reset the texture buffer
}

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
