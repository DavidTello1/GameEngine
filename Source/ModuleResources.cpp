#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
//#include "ModuleTextures.h"
#include "Mesh.h"
//#include "ResourceTexture.h"
//#include "ResourceMaterial.h"
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
	for (int i = 0; i < meshes.size(); i++) //meshes
	{
		delete meshes[i];
	}
	meshes.clear();

	return true;
}

void ModuleResources::Draw()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindVertexArray(meshes[i]->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->IBO);

		glDrawElements(GL_TRIANGLES, meshes[i]->num_indices, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

//---------------------------------
Resources::Type ModuleResources::GetResourceType(const char* path)
{
	char extension[32];
	const char* last_dot = strrchr(path, '.');
	strcpy(extension, last_dot + 1);

	for (int i = 0; i < strlen(extension); i++)
	{
		extension[i] = tolower(extension[i]);
	}

	if (extension == "obj" || strcmp("fbx",extension) == 0) // Mesh
	{
		return Resources::Type::mesh; 
	}
	else if (extension == "dds") // Texture
	{
		return Resources::Type::texture;
	}

	LOG("File format not supported", 'e');
	return Resources::Type::unknown;
}

void ModuleResources::LoadResource(const char* path, Resources::Type type)
{
	if (type == Resources::Type::unknown)
	{
		type = GetResourceType(path);
	}
	
	if (type == Resources::Type::mesh) // Mesh
	{
		const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene != nullptr && scene->HasMeshes())
		{
			for (uint i = 0; i < scene->mNumMeshes; ++i)
			{
				Mesh* new_mesh = new Mesh();
				meshes.push_back(new_mesh);

				aiMesh* mesh = scene->mMeshes[i];
				new_mesh->ImportMesh(mesh);
			}
			aiReleaseImport(scene);
		}
		else
			LOG("Error loading mesh %s", path, 'e');

	}
	else if (type == Resources::Type::texture) // Texture
	{

	}
}

void ModuleResources::UnLoadResource()
{

}