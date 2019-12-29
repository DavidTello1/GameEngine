 #ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include <vector>
#include <map>
#include <string>

#include "glew\include\GL\glew.h"

class Resource;
class ResourceMesh;
class ResourceMaterial;
struct PathNode;

struct MetaFile
{
	UID uid = 0;
	Resource::Type type = Resource::unknown;
  	std::string original_file = "";

	bool Compare(UID id, const char* file, Resource::Type type)
	{
		return (uid == id && original_file == file && type == this->type);
	}
};

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	// Importing
	bool ImportFromPath(const char* path, UID uid = 0);
	bool ImportResource(const char* path, UID uid = 0);
	bool CheckLoaded(std::string path, UID uid);

	// Resources
	Resource* CreateResource(Resource::Type type, const char* path = nullptr, UID uid = 0);
	void RemoveResource(UID uid);
	const Resource* GetResource(UID uid) const;
	Resource* GetResource(UID uid);
	Resource::Type GetResourceType(const char* path) const;

	uint GetNumResources() { return resources.size(); }
	std::vector<Resource*> GetAllResourcesOfType(Resource::Type type);
	const char* GetDirectory(Resource::Type type) const;

	// Meta files
	void SaveMeta(const Resource* resource);
	UID GetIDFromMeta(const char* path);

private:
	void LoadCheckersTexture();

	void LoadAssetsIcons();
	void UpdateAssets();
	void UpdateAssetsFolder(const PathNode& node);

public:
	ResourceMaterial* checkers_texture = nullptr;

	GLuint bbox_indices[24] =
	{
		0,1,2,3,0,2,1,3,
		4,5,6,7,4,6,5,7,
		0,4,1,5,2,6,3,7
	};

private:
	std::map<UID, Resource*> resources;

};

extern GLuint aabb_indices[24];
extern GLuint aabb_IBO;

#endif