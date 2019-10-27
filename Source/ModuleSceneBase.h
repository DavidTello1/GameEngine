#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ModuleSceneBase : public Module
{
public:
	ModuleSceneBase(bool start_enabled = true);
	~ModuleSceneBase();

	bool Start(Config* config = nullptr);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	bool Draw();

private:

	void DrawGridPlane();
	void DrawAxis();
};

