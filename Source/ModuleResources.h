#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include <vector>
#include "Module.h"
#include "Resources.h"

class Mesh;

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	Resources::Type GetResourceType(const char* path);
	void LoadResource(const char* path, Resources::Type type = Resources::Type::unknown, bool use = false);
	void LogImageInfo();
	GLuint ImportTexture(int width, int height, int internal_format, int format, unsigned char * image);
	void UnLoadResource();

	static GLuint checker_texture;
	std::vector<Mesh*> meshes;
	std::vector<GLuint> textures;

private:
	void MakeCheckersTexture();
};

#endif