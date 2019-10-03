#pragma once
#include "Module.h"
#include "Globals.h"

#define MAX_SNAKE 2

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

};
