#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "Assets.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceScene.h"
#include "Config.h"
#include "PathNode.h"

#include "Assimp/include/cimport.h"

#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"

#include "mmgr/mmgr.h"

GLuint aabb_indices[24]=
{
	0,1,2,3,0,2,1,3,
	4,5,6,7,4,6,5,7,
	0,4,1,5,2,6,3,7
};

GLuint aabb_IBO = 0;

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
	LoadCheckersTexture();
	LoadAssetsIcons();
	UpdateAssets();

	// Creation of the Index Buffer Object of the bounding boxes array, as all uses the same
	if (aabb_IBO == 0) glGenBuffers(1, &aabb_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabb_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aabb_indices), aabb_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool ModuleResources::CleanUp()
{
	LOG("Unloading Resource Manager");

	//SaveResources();

	if (aabb_IBO != 0)
	{
		glDeleteBuffers(1, &aabb_IBO);
	}

	for (std::map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);
	resources.clear();

	return true;
}

bool ModuleResources::ImportFromPath(const char* path, UID uid)
{
	std::string full_path = path;
	App->file_system->NormalizePath(full_path);
	std::string final_path = App->file_system->GetFileName(full_path.c_str()); //get file name
	final_path = ASSETS_FOLDER + final_path;

	App->file_system->NormalizePath(final_path);

	if (App->file_system->CopyFromOutside(path, final_path.c_str()) == true) //copy file to final_path
		return ImportResource(final_path.c_str(), uid);

	return false;
}

bool ModuleResources::ImportResource(const char* final_path, UID uid)
{
	Resource::Type type = GetResourceType(final_path); //get resource type

	if (CheckLoaded(final_path, uid) == true) // Check if file has already been loaded
	{
		CreateResource(type, final_path, uid);

		// ***if last mod date not equal reimport
		LOG("File has already been imported", 'd');
		return true;
	}


	if (type == Resource::Type::model)
	{
		ResourceModel* model = (ResourceModel*)CreateResource(type, final_path, uid);
		if (model->Import(final_path))
		{
			SaveMeta((Resource*)model);
			return true;
		}
	}
	else if (type == Resource::Type::material)
	{
		ResourceMaterial* material = (ResourceMaterial*)CreateResource(type, final_path, uid);
		if (material->Import(final_path))
		{
			SaveMeta((Resource*)material);
			return true;
		}
	}
	else if (type == Resource::Type::mesh)
	{
		ResourceScene* scene = (ResourceScene*)CreateResource(type, final_path, uid);
		if (scene->Import(final_path))
		{
			SaveMeta((Resource*)scene);
			return true;
		}
	}

	LOG("Importing of [%s] FAILED", final_path);
	return false;
}

Resource* ModuleResources::CreateResource(Resource::Type type, const char* path, UID force_uid)
{
	Resource* ret = nullptr;
	UID uid;

	if (force_uid != 0 && GetResource(force_uid) == nullptr)
		uid = force_uid;
	else
		uid = App->random->Int();

	switch (type)
	{
	case Resource::unknown:
		break;
	case Resource::model:
		ret = new ResourceModel(uid);
		break;
	case Resource::mesh:
		ret = new ResourceMesh(uid);
		break;
	case Resource::material:
		ret = new ResourceMaterial(uid);
		break;
	case Resource::scene:
		ret = new ResourceScene(uid);
		break;
	}

	if (ret != nullptr)
	{
		if (path != nullptr)
			ret->original_file = path;

		resources[uid] = ret;
	}

	return ret;
}

void ModuleResources::RemoveResource(UID uid)
{
	std::map<UID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		App->file_system->Remove(it->second->GetExportedFile().c_str());

		char tmp[256];
		sprintf_s(tmp, 255, "%s%s", GetDirectory(it->second->GetType()), it->second->GetExportedFile());
		App->file_system->Remove(tmp);

		resources.erase(it);
	}
}

const Resource* ModuleResources::GetResource(UID uid) const
{
	if (resources.find(uid) != resources.end())
		return resources.at(uid);
	return nullptr;
}

Resource* ModuleResources::GetResource(UID uid)
{
	std::map<UID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;
	return nullptr;
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
	else if (strcmp("dds", extension.c_str()) == 0 || strcmp("png", extension.c_str()) == 0 || 
			 strcmp("jpg", extension.c_str()) == 0 || strcmp("tga", extension.c_str()) == 0)
	{
		type = Resource::Type::material;
		LOG("Importing resource material from %s", path, 'd');
	}
	else if (strcmp("dvs", extension.c_str()) == 0)
	{
		type = Resource::Type::scene;
		LOG("Importing resource scene from %s", path, 'd');
	}
	else
	{
		type = Resource::Type::unknown;
		LOG("%s format not supported from %s", extension.c_str(), path, 'e');
	}

	return type;
}

std::vector<Resource*> ModuleResources::GetAllResourcesOfType(Resource::Type type)
{
	std::vector<Resource*> ret;

	std::map<UID, Resource*>::iterator it = resources.begin();
	for (; it != resources.end(); it++)
	{
		if (it->second->GetType() == type)
			ret.push_back(it->second);
	}
	return ret;
}

bool ModuleResources::CheckLoaded(std::string path, UID uid)
{
	Resource::Type type = GetResourceType(path.c_str());
	std::string file;

	switch (type)
	{
	case Resource::unknown:
		break;
	case Resource::model:
		file = LIBRARY_MODEL_FOLDER + std::to_string(uid) + ".dvs_model";
		break;
	case Resource::mesh:
		file = LIBRARY_MESH_FOLDER + std::to_string(uid) + ".dvs_mesh";
		break;
	case Resource::material:
		file = LIBRARY_MATERIAL_FOLDER + std::to_string(uid) + ".dvs_material";
		break;
	case Resource::scene:
		file = LIBRARY_SCENE_FOLDER + std::to_string(uid) + ".dvs_scene";
		break;
	}

	if (App->file_system->Exists(file.c_str()))
	{
		return true;
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
	if (node.file == true) //If node is a file
	{
		UID id = 0;
		std::string meta(node.path + (".meta"));

		if (App->file_system->Exists(meta.c_str())) //check if meta file has already been created and init UID if so
			id = App->resources->GetIDFromMeta(meta.c_str());

		ImportResource(node.path.c_str(), id);
	}
	else if (node.leaf == false) //If node folder has something inside
	{
		for (uint i = 0; i < node.children.size(); i++)
			UpdateAssetsFolder(node.children[i]);
	}
}

void ModuleResources::LoadAssetsIcons()
{
	ResourceMaterial* resource = (ResourceMaterial*)CreateResource(Resource::Type::material);
	App->editor->tab_assets->folder_icon = resource->LoadTexture("Settings/Icons/folder_icon.png");

	resource = (ResourceMaterial*)CreateResource(Resource::Type::material);
	App->editor->tab_assets->file_icon = resource->LoadTexture("Settings/Icons/file_icon.png");

	resource = (ResourceMaterial*)CreateResource(Resource::Type::material);
	App->editor->tab_assets->model_icon = resource->LoadTexture("Settings/Icons/model_icon.png");

	resource = (ResourceMaterial*)CreateResource(Resource::Type::material);
	App->editor->tab_assets->material_icon = resource->LoadTexture("Settings/Icons/material_icon.png");

	resource = (ResourceMaterial*)CreateResource(Resource::Type::material);
	App->editor->tab_assets->scene_icon = resource->LoadTexture("Settings/Icons/scene_icon.png");
}

void ModuleResources::LoadCheckersTexture()
{
	const int checkImageWidth = 256;
	const int checkImageHeight = 256;

	checkers_texture = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	checkers_texture->tex_width = checkImageWidth;
	checkers_texture->tex_height = checkImageHeight;

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

	glGenTextures(1, &checkers_texture->tex_id);
	glBindTexture(GL_TEXTURE_2D, checkers_texture->tex_id);

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

UID ModuleResources::GetIDFromMeta(const char* path)
{
	char* buffer = nullptr;
	uint size = App->file_system->Load(path, &buffer);

	if (size > 0)
	{
		UID ret = Config(buffer).GetNumber("ID");
		delete[] buffer;
		return ret;
	}

	delete[] buffer;
	return 0;
}

void ModuleResources::SaveMeta(const Resource* resource)
{
	Config config;

	config.SetNumber("ID", resource->uid);
	config.SetString("File", resource->original_file.c_str());
	config.SetNumber("Type", static_cast<int>(resource->GetType()));

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	if (size > 0)
	{
		std::string path = resource->original_file + ".meta";
		App->file_system->Save(path.c_str(), buffer, size);
	}
}
