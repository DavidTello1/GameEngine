#pragma once

#include "Globals.h"
#include "Module.h"
#include "assimp/include/cimport.h" 
#include "assimp/include/scene.h" 
#include "assimp/include/postprocess.h" 
#include "assimp/include/cfileio.h"

#pragma comment (lib, "assimp/libx86/assimp.lib")


struct MeshData {
	uint id_index = 0; // index in VRAM 
	uint num_index = 0; 
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM 
	uint num_vertex = 0; 
	float* vertex = nullptr;
};

class GeometryLoader : public Module
{
public:

	GeometryLoader(bool start_enabled = true);
	~GeometryLoader();


	bool Start();
	static MeshData* LoadModel(const char * path);
	bool Update(float dt);
	bool CleanUp();

};

