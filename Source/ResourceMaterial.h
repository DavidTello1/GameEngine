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

	UID Import(const char* source_file, const aiMaterial* material = nullptr);
	bool SaveOwnFormat() const;
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

