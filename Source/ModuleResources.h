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
struct PathNode;

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	UID GenerateUID();
	bool ImportFromOutside(const char* path, UID uid = 0);
	bool ImportResource(const char* path, UID uid = 0);
	bool CheckLoaded(std::string path, UID uid);

	Resource* CreateResource(Resource::Type type, UID uid = 0);
	Resource* CreateInitResource(Resource::Type type, UID uid, const char* path, std::string& written_file);
	void RemoveResource(UID uid);
	const Resource* GetResource(UID uid) const;
	Resource * GetResource(UID uid);
	Resource::Type GetResourceType(const char* path) const;

	const char* GetDirectory(Resource::Type type) const;
	bool IsResourcesEmpty() { return resources.empty(); }

private:
	void LoadUID();
	void SaveUID() const;

	//void MakeCheckersTexture();

	void LoadAssetsIcons();
	void UpdateAssets();
	void UpdateAssetsFolder(const PathNode& node);

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

	UID last_uid = RESERVED_RESOURCES;
};

#endif