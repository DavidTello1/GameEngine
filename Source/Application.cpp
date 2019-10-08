#include "Application.h"

using namespace std;

Application::Application()
{
	frames = 0;
	last_frame_ms = -1;
	last_fps = -1;
	capped_ms = 1000 / 60;
	fps_counter = 0;

	//modules.push_back(hints = new ModuleHints());
	//modules.push_back(hw = new ModuleHardware(false));
	//modules.push_back(fs = new ModuleFileSystem(ASSETS_FOLDER));
	modules.push_back(window = new ModuleWindow());
	//modules.push_back(resources = new ModuleResources());
	//modules.push_back(tex = new ModuleTextures());
	//modules.push_back(physics3D = new ModulePhysics3D());
	modules.push_back(camera = new ModuleCamera3D());
	modules.push_back(renderer3D = new ModuleRenderer3D());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(input = new ModuleInput());
	modules.push_back(viewport = new ModuleViewport());
	//modules.push_back(audio = new ModuleAudio(true));
	//modules.push_back(ai = new ModuleAI());
	//modules.push_back(level = new ModuleLevelManager());
	//modules.push_back(programs = new ModulePrograms(true));
	//modules.push_back(renderer = new ModuleRenderer());
	//modules.push_back(debug_draw = new ModuleDebugDraw());
}

// ---------------------------------------------
Application::~Application()
{
	for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
	{
		RELEASE(*it);
	}
}

// ---------------------------------------------
bool Application::Init()
{
	bool ret = true;

	char* buffer = nullptr;

	// We init everything, even if not enabled
	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Init();
	}

	// Another round, just before starting the Updates. Only called for "active" modules
	// we send the configuration again in case a module needs it
	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		if ((*it)->IsActive() == true)
			ret = (*it)->Start();
	}

	RELEASE_ARRAY(buffer);
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
bool Application::Update()
{
	bool ret = true;
	PrepareUpdate();

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->PreUpdate(dt);

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->Update(dt);

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->PostUpdate(dt);

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	// Recap on framecount and fps
	++frames;
	++fps_counter;

	if (fps_timer.Read() >= 1000)
	{
		last_fps = fps_counter;
		fps_counter = 0;
		fps_timer.Start();
	}

	last_frame_ms = ms_timer.Read();

	// cap fps
	if (capped_ms > 0 && (last_frame_ms < capped_ms))
		SDL_Delay(capped_ms - last_frame_ms);

	// notify the editor
	editor->LogFPS((float)last_fps, (float)last_frame_ms);
}

// ---------------------------------------------
bool Application::CleanUp()
{
	bool ret = true;

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->CleanUp();

	return ret;
}

// ---------------------------------------------
const char* Application::GetAppName() const
{
	return app_name.c_str();
}

// ---------------------------------------------
void Application::SetAppName(const char * name)
{
	if (name != nullptr && name != app_name)
	{
		app_name = name;
		window->SetTitle(name);
		// TODO: Filesystem should adjust its writing folder
	}
}

// ---------------------------------------------
const char* Application::GetOrganizationName() const
{
	return organization_name.c_str();
}

void Application::SetOrganizationName(const char * name)
{
	if (name != nullptr && name != organization_name)
	{
		organization_name = name;
		// TODO: Filesystem should adjust its writing folder
	}
}


// ---------------------------------------------
uint Application::GetFramerateLimit() const
{
	if (capped_ms > 0)
		return (uint)((1.0f / (float)capped_ms) * 1000.0f);
	else
		return 0;
}

// ---------------------------------------------
void Application::SetFramerateLimit(uint max_framerate)
{
	if (max_framerate > 0)
		capped_ms = 1000 / max_framerate;
	else
		capped_ms = 0;
}