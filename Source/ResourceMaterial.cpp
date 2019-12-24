#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ResourceMaterial.h"
#include "ComponentMaterial.h"

#include "Assimp/include/types.h"
#include "Assimp/include/material.h"

#include "Devil/include/IL/il.h"
#pragma comment (lib, "Devil/lib/x86/DevIL.lib")
#pragma comment (lib, "Devil/lib/x86/ILU.lib")
#pragma comment (lib, "Devil/lib/x86/ILUT.lib")

#include "SimpleBinStream.h"

#include "mmgr/mmgr.h"

ResourceMaterial::ResourceMaterial(UID id) : Resource(id, Resource::Type::material)
{
}

ResourceMaterial::~ResourceMaterial()
{
	UnLoad();
}

UID ResourceMaterial::Import(const char* source_file, std::string& asset_file, const aiMaterial* ai_material)
{
	ResourceMaterial* material = static_cast<ResourceMaterial*>(App->resources->CreateResource(Resource::material)); //create new material

	ResourceMaterial mat(0);
	uint ret = 0;
	ret = material->LoadTexture(source_file);
	if (ret == 0)
		LOG("Error Importing texture from '%s'", source_file, 'e');

	// Saving to own format
	if (material->SaveOwnFormat(asset_file))
	{
		material->original_file = source_file; //get file
		App->file_system->NormalizePath(material->original_file);

		LOG("Imported aiMaterial from [%s]", material->GetFile());
	}
	else
	{
		LOG("Importing aiMaterial %s FAILED", source_file);
	}

	material->UnLoad();

	return material->uid;
}

bool ResourceMaterial::ImportTexture(const char* path)
{
	ILuint size;
	ILubyte *data;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0)
	{
		data = new ILubyte[size]; // allocate data buffer   
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function       
			App->file_system->Save(path, data, size);

		RELEASE_ARRAY(data);
		return true;
	}
	return false;
}

bool ResourceMaterial::SaveOwnFormat(std::string& asset_file) const
{
	simple::mem_ostream<std::true_type> write_stream; //create asset_file stream

	write_stream << tex_id;
	write_stream << tex_height;
	write_stream << tex_width;

	const std::vector<char>& data = write_stream.get_internal_vec(); //get vector from stream

	asset_file = LIBRARY_MATERIAL_FOLDER + std::to_string(uid) + ".dvs_material";
	if (App->file_system->Save(asset_file.c_str(), &data[0], data.size()) > 0) //save file
		return true;

	return false;
}

bool ResourceMaterial::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		std::string file = LIBRARY_MATERIAL_FOLDER + std::to_string(uid) + ".dvs_material";
		uint size = App->file_system->Load(file.c_str(), &buffer); //get total size

		simple::mem_istream<std::true_type> read_stream(buffer, size); //create input stream

		read_stream >> tex_id;
		read_stream >> tex_height;
		read_stream >> tex_width;

		delete[] buffer;
		return true;
	}
	return false;
}

void ResourceMaterial::UnLoad()
{
	tex_id = NULL;
	tex_height = NULL;
	tex_width = NULL;
}


uint ResourceMaterial::LoadTexture(const char* path)
{
	uint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	bool loaded = ilLoadImage(path);
	if (!loaded)
	{
		LOG("Image %s not loaded", path, 'e');
		return false;
	}
	LogImageInfo();

	//if (ImportTexture(path) == false)
	//{
	//	LOG("Texture not imported", 'e');
	//	return false;
	//}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT),
		ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0,
		ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	glGenerateMipmap(GL_TEXTURE_2D);

	tex_width = IL_IMAGE_WIDTH;
	tex_height = IL_IMAGE_HEIGHT;
	tex_id = texture;

	ilDeleteImages(1, &imageID);
	return tex_id;
}

void ResourceMaterial::LogImageInfo()
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