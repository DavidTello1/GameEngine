#include "Application.h"

#include "ModuleScene.h"

#include "mmgr/mmgr.h"

using namespace std;

Application::Application()
{
	frames = 0;
	last_frame_ms = -1;
	last_fps = -1;
	capped_ms = 1000 / 60;
	fps_counter = 0;
	random = new math::LCG();

	modules.push_back(file_system = new ModuleFileSystem(ASSETS_FOLDER));
	modules.push_back(window = new ModuleWindow());
	modules.push_back(resources = new ModuleResources());
	modules.push_back(gui = new ModuleGUI());
	modules.push_back(scene_base = new ModuleSceneBase());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(input = new ModuleInput());
	modules.push_back(renderer3D = new ModuleRenderer3D());
}

// ---------------------------------------------
Application::~Application()
{
	for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		RELEASE(*it);
	
	RELEASE(random);
}

// ---------------------------------------------
bool Application::Init()
{
	bool ret = true;
	char* buffer = nullptr;

	file_system->Load(SETTINGS_FOLDER "config.json", &buffer);

	Config config((const char*)buffer);
	ReadConfiguration(config.GetNode("App"));

	// We init everything, even if not enabled
	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Init(&(config.GetNode((*it)->GetName())));
	}

	// Another round, just before starting the Updates. Only called for "active" modules
	// we send the configuration again in case a module needs it
	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		if ((*it)->IsActive() == true)
			ret = (*it)->Start(&(config.GetNode((*it)->GetName())));
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

void Application::ReadConfiguration(const Config& config)
{
	SetAppName(config.GetString("Name", "Davos Game Engine").c_str());
	organization_name = config.GetString("Organization", "");
	SetFramerateLimit(config.GetNumber("MaxFramerate", 0));
}

void Application::SaveConfiguration(Config& config) const
{
	config.SetString("Name", app_name.c_str());
	config.SetString("Organization", organization_name.c_str());
	config.SetNumber("MaxFramerate", GetFramerateLimit());
}

string Application::GetLog()
{
	return log;
}

void Application::LoadPrefs()
{
	char* buffer = nullptr;
	uint size = file_system->Load(SETTINGS_FOLDER "config.json", &buffer);

	if (size > 0)
	{
		Config config((const char*)buffer);

		ReadConfiguration(config.GetNode("App"));
		for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		{
			(*it)->Load(&config.GetNode((*it)->GetName()));
		}

		RELEASE_ARRAY(buffer);
	}
}

// ---------------------------------------------
void Application::SavePrefs() const
{
	Config config;

	SaveConfiguration(config.SetNode("App"));

	for (list<Module*>::const_iterator it = modules.begin(); it != modules.end(); ++it)
		(*it)->Save(&config.SetNode((*it)->GetName()));

	char *buf;
	uint size = config.Serialize(&buf);
	if (file_system->Save(SETTINGS_FOLDER "config.json", buf, size) > 0) {}
		//LOG("Saved Engine Preferences")
	RELEASE_ARRAY(buf);
}