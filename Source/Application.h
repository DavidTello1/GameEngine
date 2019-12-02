#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneBase.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Config.h"

#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include <list>

class ModuleScene;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Update();
	bool CleanUp();

	const char* GetAppName() const;
	void SetAppName(const char* name);
	const char* GetOrganizationName() const;
	void SetOrganizationName(const char* name);
	uint GetFramerateLimit() const;
	void SetFramerateLimit(uint max_framerate);

	void ReadConfiguration(const Config& config);
	void SaveConfiguration(Config& config) const;

	std::string GetLog();
	void LoadPrefs();
	void SavePrefs() const;

	LCG& Random() { return *random; }

private:
	void PrepareUpdate();
	void FinishUpdate();


public:
	LCG* random = nullptr;

	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneBase* scene_base;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleEditor* editor;
	ModuleFileSystem* file_system;
	ModuleResources* resources;

private:
	Timer	ms_timer;
	Timer	fps_timer;
	Uint32	frames;
	float	dt;
	int		fps_counter;
	int		last_frame_ms;
	int		last_fps;
	int		capped_ms;

	std::list<Module*> modules;
	std::string log;
	std::string app_name;
	std::string organization_name;
};

extern Application* App;
