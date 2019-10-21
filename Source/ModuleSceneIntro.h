#pragma once
#include "Module.h"
#include "Globals.h"
#include "Mesh.h"

#define MAX_SNAKE 2

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start(Config* config = nullptr);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	
	bool Draw();

private:
	void DrawCube();
	void DrawGridPlane();
	void DrawAxis();
};

