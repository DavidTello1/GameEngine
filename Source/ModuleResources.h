#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"

#include <vector>
#include <map>
#include <string>

#include "glew\include\GL\glew.h"

class Resource;
class ResourceMesh;
class ResourceMaterial;

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	bool ImportResource(const char* path, UID uid = 0);

	Resource* CreateResource(Resource::Type type, UID uid = 0);
	void RemoveResource(UID uid);

	const Resource* GetResource(UID uid) const;
	Resource * GetResource(UID uid);

	UID GenerateUID();

private:
	void MakeCheckersTexture();

public:
	GLuint bbox_indices[24] =
	{
		0,1,2,3,0,3,1,2,
		4,5,6,7,4,7,5,6,
		0,4,1,5,2,6,3,7
	};

private:
	std::map<UID, Resource*> resources;
	std::vector<Resource*> removed;

};

#endif