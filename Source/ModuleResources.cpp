#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
//#include "ModuleTextures.h"
//#include "ResourceTexture.h"
//#include "ResourceMaterial.h"
#include "Mesh.h"
//#include "ResourceAudio.h"
//#include "ResourceModel.h"
//#include "ResourceAnimation.h"
//#include "ResourceStateMachine.h"
#include "Config.h"

#include <string>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"

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
	return true;
}

bool ModuleResources::CleanUp()
{
	return true;
}

void ModuleResources::Draw()
{

}

Mesh* ModuleResources::LoadFBX(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		Mesh* new_mesh = new Mesh();
		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			meshes.push_back(new_mesh);

			aiMesh* mesh = scene->mMeshes[i];
			new_mesh->ImportMesh(mesh);
		}
		aiReleaseImport(scene);
		return new_mesh;
	}
	else
		LOG("Error loading scene %s", path, 'd');

}
