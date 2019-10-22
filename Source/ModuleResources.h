#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include <vector>
#include "Module.h"
#include "Resources.h"

class Mesh;

class ModuleResources : public Module
{
public:

	void MakeCheckTexture();

	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	void Draw();

	Resources::Type GetResourceType(const char* path);
	void LoadResource(const char* path, Resources::Type type = Resources::Type::unknown);
	void UnLoadResource();

private:
	std::vector<Mesh*> meshes;

	static GLuint checker_texture;
};

#endif