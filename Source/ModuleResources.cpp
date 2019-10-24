#include "Globals.h"
#include "Application.h"
#include "ModuleSceneBase.h"
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

/*  Create checkerboard texture  */
#define checkImageWidth 256
#define checkImageHeight 256
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
GLuint ModuleResources::checker_texture;

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

void AssimpLogCallback(const char *msg, char *userData) {
	LOG("%s", msg,'g');
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
Resources::Type ModuleResources::GetResourceType(const char* path)
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
		return Resources::Type::mesh; 
	}
	else if (strcmp("dds", extension) == 0 || strcmp("png", extension) == 0 || strcmp("jpg", extension) == 0) // Texture
	{
		return Resources::Type::texture;
	}

	LOG("File format not supported", 'e');
	return Resources::Type::unknown;
}

void ModuleResources::LoadResource(const char* path, Resources::Type type, bool use)
{
	if (type == Resources::Type::unknown)
		type = GetResourceType(path);

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

				if (use)
				{
					//name
					const char* pos = strrchr(path, 92);
					if (pos == nullptr) pos = strrchr(path, '/');

					GameObject* o = App->scene->CreateGameObj((pos == nullptr) ? "Unkown" : pos + 1);
					o->SetMesh(App->resources->meshes.back());
				}

			}
			aiReleaseImport(scene);
		}
		else
			LOG("Error loading mesh %s", path, 'e');

	}
	else if (type == Resources::Type::texture) // Texture
	{
		// Devil
		unsigned int imageID;
		ilGenImages(1, &imageID);
		ilBindImage(imageID);

		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

		bool loaded = ilLoadImage(path);
		if (!loaded) LOG("IMAGE '%s' COULD NOT BE LOADED PROPERLY", path, 'e');

		LogImageInfo();

		GLuint tex = ImportTexture(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
		ilDeleteImages(1, &imageID);

		textures.push_back(tex);

		if (use)
		{
			App->scene->selected_gameobj->GetMesh()->TEX = tex;
		}

	}
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
	LOG("Original image format: %s, data type: %s", s,c, 'g');
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

void ModuleResources::MakeCheckersTexture()
{
	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
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
