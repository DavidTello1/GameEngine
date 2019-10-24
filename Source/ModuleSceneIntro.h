#pragma once
#include "Module.h"
#include "Globals.h"
#include "Mesh.h"
#include "GameObject.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

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

	void DrawGridPlane();
	void DrawAxis();


};

