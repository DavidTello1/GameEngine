#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleViewport.h"

#include <list>

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

	//void LoadPrefs();
	//void SavePrefs() const;

private:
	void PrepareUpdate();
	void FinishUpdate();


public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleViewport* viewport;

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
