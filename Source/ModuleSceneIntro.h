#pragma once
#include "Module.h"
#include "Globals.h"
#include "Mesh.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"


#define MAX_SNAKE 2

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start(Config* config = nullptr);
	void CreateShape(shape_type type, int slices, int stacks, float x = 0, float y = 0, float z = 0);
	bool Update(float dt);
	bool Draw();
	bool PostUpdate(float dt);
	bool CleanUp();

	//Temporary mesh data
	Mesh* m;
};

void DrawGridPlane();
