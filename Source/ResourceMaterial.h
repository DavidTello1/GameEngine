#pragma once
#include "Resource.h"

struct aiMaterial;

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(UID id);
	virtual ~ResourceMaterial();

	static UID Import(const aiMaterial* material, const char* source_file);
	bool SaveOwnFormat(std::string& output) const;
	bool LoadtoScene();
	void UnLoad();

private:
	bool LoadMaterial(const char* path);

	//GLuint ImportTexture(int width, int height, int internal_format, int format, unsigned char * image);
	void LogImageInfo();

public:
	GLuint checker_texture;

private:
	uint tex_height, tex_width;

};

