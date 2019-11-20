#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceMaterial.h"

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
}

UID ResourceMaterial::Import(const char* source_file, const aiMaterial* ai_material)
{
	ResourceMaterial* material = static_cast<ResourceMaterial*>(App->resources->CreateResource(Resource::material)); //create new material

	ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, material->diffuse_color);
	ai_material->Get(AI_MATKEY_COLOR_SPECULAR, material->specular_color);
	ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, material->emissive_color);
	ai_material->Get(AI_MATKEY_SHININESS, material->shininess);

	aiString file;
	aiTextureMapping mapping;
	unsigned uvindex = 0;
	if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, &uvindex) == AI_SUCCESS)
	{
		assert(mapping == aiTextureMapping_UV);
		assert(uvindex == 0);

		std::string full_path(source_file);
		full_path.append(file.data);

		material->textures[TextureDiffuse] = ImportTexture(full_path);
	}

	// Saving to own format
	std::string output;
	if (material->SaveOwnFormat(output))
	{
		material->original_file = source_file; //get file
		App->file_system->NormalizePath(material->original_file);

		std::string file_name = App->file_system->GetFileName(output.c_str());//get exported file
		material->exported_file = file_name;

		LOG("Imported aiMaterial from [%s] to [%s]", material->GetFile(), material->GetExportedFile());
	}
	else
	{
		LOG("Importing aiMaterial %s FAILED", source_file);
	}

	return material->uid;
}

UID ResourceMaterial::ImportTexture(std::string& output)
{
	bool ret = false;
	ILuint size;
	ILubyte *data;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer

	if (size > 0)
	{
		data = new ILubyte[size]; // allocate data buffer   

		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function       
		{
			ret = App->file_system->SaveUnique(output, data, size, LIBRARY_MATERIAL_FOLDER, "material", "dvs_material");
		}

		RELEASE_ARRAY(data);
		return ret;
	}
	return false;
}

bool ResourceMaterial::SaveOwnFormat(std::string& output) const
{
	simple::mem_ostream<std::true_type> write_stream; //create output stream

	// Store Colors
	write_stream << diffuse_color.x << diffuse_color.y << diffuse_color.z << diffuse_color.w;
	write_stream << specular_color.x << specular_color.y << specular_color.z;
	write_stream << emissive_color.x << emissive_color.y << emissive_color.z;

	// Store Textures
	for (uint i = 0; i < TextureCount; ++i)
	{
		write_stream << textures[i];
	}

	// Store rest
	write_stream << k_ambient << k_diffuse << k_specular;
	write_stream << shininess;

	const std::vector<char>& data = write_stream.get_internal_vec(); //get vector from stream

	return App->file_system->SaveUnique(output, &data[0], data.size(), LIBRARY_MATERIAL_FOLDER, "material", "dvs_material"); //save
}

bool ResourceMaterial::LoadtoScene()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		uint size = App->file_system->LoadFromPath(LIBRARY_MATERIAL_FOLDER, GetExportedFile(), &buffer);

		simple::mem_istream<std::true_type> read_stream(buffer, size); //create input stream

		read_stream >> diffuse_color.x >> diffuse_color.y >> diffuse_color.z >> diffuse_color.w;
		read_stream >> specular_color.x >> specular_color.y >> specular_color.z;
		read_stream >> emissive_color.x >> emissive_color.y >> emissive_color.z;

		for (uint i = 0; i < TextureCount; ++i)
		{
			read_stream >> textures[i];
		}

		read_stream >> k_ambient >> k_diffuse >> k_specular;
		read_stream >> shininess;

		delete[] buffer;
		return true;
	}
	return false;
}

void ResourceMaterial::UnLoad()
{
	for (uint i = 0; i < TextureCount; ++i)
	{
		if (textures[i] != 0)
		{
			App->resources->GetResource(textures[i])->ReleaseFromMemory();
			textures[i] = 0;
		}
	}
}


bool ResourceMaterial::LoadMaterial(const aiMaterial* material, const char* path)
{
	// Devil
	uint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	bool loaded = ilLoadImage(path);
	if (!loaded)
	{
		LOG("IMAGE '%s' COULD NOT BE LOADED PROPERLY", path, 'e');
		return false;
	}

	LogImageInfo();

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

	ilDeleteImages(1, &imageID);
	return true;
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