#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include <vector>
#include "Module.h"

class Mesh;

class ModuleResources : public Module
{
public:

	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	void Draw();
	bool LoadFBX(const char* path);

private:
	std::vector<Mesh*> meshes;

};

#endif