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

	static UID Import(const char* source_file, std::string& asset_file, const aiMaterial* material = nullptr);
	bool SaveOwnFormat(std::string& asset_file) const;
	bool LoadtoScene();
	void UnLoad();

	bool ImportTexture(const char* path);
	uint LoadTexture(const char* path);

private:
	void LogImageInfo();

public:
	uint tex_id = 0;
	uint tex_width = 0;
	uint tex_height = 0;

};

