#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"

#include <vector>
#include <map>
#include <string>

#include "glew\include\GL\glew.h"

#define RESERVED_RESOURCES 1 // checker texture

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

	UID GenerateUID();
	bool ImportResource(const char* path, UID uid = 0);

	Resource* CreateResource(Resource::Type type, UID uid = 0);
	Resource* CreateInitResource(Resource::Type type, UID uid, std::string& path, std::string& written_file);
	void RemoveResource(UID uid);
	const Resource* GetResource(UID uid) const;
	Resource * GetResource(UID uid);
	Resource::Type GetResourceType(const char* path) const;
	bool CheckLoaded(std::string path, UID uid);

	const char* GetDirectory(Resource::Type type) const;

private:
	void LoadUID();
	void SaveUID() const;

	//void MakeCheckersTexture();

private:
	std::map<UID, Resource*> resources;
	std::vector<Resource*> removed;

	UID last_uid = RESERVED_RESOURCES;
};

extern GLuint aabb_indices[24];
extern GLuint aabb_IBO;

#endif