#pragma once
#include "Resource.h"
#include "Math.h"
#include "Color.h"

struct aiMaterial;

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(UID id);
	virtual ~ResourceMaterial();

	static UID Import(const char* source_file, const aiMaterial* material = nullptr);
	bool ImportTexture(const char* path);
	bool SaveOwnFormat(std::string& output) const;
	bool LoadtoScene();
	void UnLoad();

private:
	bool LoadMaterial(const char* path);

	//GLuint ImportTexture(int width, int height, int internal_format, int format, unsigned char * image);
	void LogImageInfo();

public:
	//GLuint checker_texture;

public:
	uint tex_id = 0;
	uint tex_width = 0;
	uint tex_height = 0;

};

