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

#include "Devil/include/IL/il.h"
#pragma comment (lib, "Devil/lib/x86/DevIL.lib")
#pragma comment (lib, "Devil/lib/x86/ILU.lib")
#pragma comment (lib, "Devil/lib/x86/ILUT.lib")

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
	ilInit();
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

	for (const Mesh* m : meshes)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindTexture(GL_TEXTURE_2D, m->TEX);

		glBindBuffer(GL_ARRAY_BUFFER, m->tex_coords_id);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->IBO);
		glDrawElements(GL_TRIANGLES, m->num_indices, GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
	}
	/*for (int i = 0; i < meshes.size(); i++)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[i]->VBO);
		glBindVertexArray(meshes[i]->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->IBO);

		glBindTexture(GL_TEXTURE_2D, meshes[i]->TEX);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[i]->tex_coords_id);

		glDrawElements(GL_TRIANGLES, meshes[i]->num_indices, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}*/
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
	else if (strcmp("dds", extension) == 0 || strcmp("png", extension) == 0 || strcmp("jpg", extension) == 0) // Texture
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
		LOG("Mesh resource type",'g');
		const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene != nullptr && scene->HasMeshes())
		{
			LOG("Number of meshes: %u", scene->mNumMeshes, 'g');

			for (uint i = 0; i < scene->mNumMeshes; ++i)
			{
				Mesh* new_mesh = new Mesh();
				aiMesh* mesh = scene->mMeshes[i];

				new_mesh->ImportMesh(mesh);
				meshes.push_back(new_mesh);
			}
			aiReleaseImport(scene);
		}
		else
			LOG("Error loading mesh %s", path, 'e');

	}
	else if (type == Resources::Type::texture) // Texture
	{
		LOG("Resource type is texture", 'd');
		meshes.back()->ImportTexture(path);
		(*(meshes.begin()))->TEX = meshes.back()->TEX;
	}
}

void ModuleResources::UnLoadResource()
{

}