#pragma once
#include "Resource.h"

struct aiMaterial;

class ResourceMaterial : public Resource
{
public:
	enum Texture
	{
		TextureDiffuse = 0,
		TextureSpecular,
		TextureNormal,
		TextureOcclusion,
		TextureEmissive,
		TextureCount
	};

	ResourceMaterial(UID id);
	virtual ~ResourceMaterial();

	static UID Import(const char* source_file, const aiMaterial* material = nullptr);
	static UID ImportTexture(std::string& output);
	bool SaveOwnFormat(std::string& output) const;
	bool LoadtoScene();
	void UnLoad();

private:
	bool LoadMaterial(const aiMaterial* material, const char* path);

	//GLuint ImportTexture(int width, int height, int internal_format, int format, unsigned char * image);
	void LogImageInfo();

public:
	GLuint checker_texture;

private:

	float4      diffuse_color = float4::one;
	float3      specular_color = float3::zero;
	float3      emissive_color = float3::zero;
	UID         textures[TextureCount] = { 0, 0, 0, 0, 0 };
	float       shininess = 0.5f;
	float       k_ambient = 0.5f;
	float       k_diffuse = 0.5f;
	float       k_specular = 0.5f;
};

