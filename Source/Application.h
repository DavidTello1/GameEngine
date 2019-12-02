#pragma once
#include <list>
#include <string>

#include "Globals.h"
#include "Timer.h"

#include "MathGeoLib/include/MathGeoLibFwd.h"

class Module;
class ModuleWindow;
class ModuleScene;
class ModuleInput;
class ModuleSceneBase;
class ModuleRenderer3D;
class ModuleEditor;
class ModuleFileSystem;
class ModuleResources;

class Config;
class LCG;

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

	math::LCG& Random() { return *random; }

private:
	void PrepareUpdate();
	void FinishUpdate();


public:
	math::LCG* random = nullptr;

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
