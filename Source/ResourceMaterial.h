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

};

