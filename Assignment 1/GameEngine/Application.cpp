#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	camera = new ModuleCamera3D(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	physics = new ModulePhysics3D(this);
	editor = new ModuleEditor(this);
	scene_intro = new ModuleSceneIntro(this);
	player = new ModulePlayer(this);
	renderer3D = new ModuleRenderer3D(this);


	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(editor);
	
	// Scenes
	AddModule(scene_intro);
	AddModule(player);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	modules.clear();

	/* I think its enough doing so ^ http://www.cplusplus.com/reference/list/list/clear/
	
	std::list<Module*>::iterator item = list_modules.end();

	while(item != list_modules.begin())
	{
		delete (*item)->data;
		item = item->prev;
	}*/
}

bool Application::Init()
{
	bool ret = true;

	LOG("Application Init --------------");

	std::list<Module*>::iterator item = modules.begin();

	while(item != modules.end() && ret == true)
	{
		LOG("initializing module %s", (*item)->name)
		ret = (*item)->Init();
		item++;
	}
	if (!ret) { if (item != modules.begin()) item--; LOG("ERROR at Init, module %s", (*item)->name) }

	LOG("Application Start --------------");

	item = modules.begin();
	while(item != modules.end() && ret == true)
	{
		ret = (*item)->Start();
		item++;
	}

	if (!ret) { if (item != modules.begin())item--; LOG("ERROR at Start, module %s", (*item)->name) }

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
bool Application::Update()
{
	bool ret = true;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = modules.begin();
	
	while(item != modules.end() && ret == true)
	{
		ret = (*item)->PreUpdate(dt);
		item++;
	}
	// Error checking
	if (!ret && !input->quit) { if(item != modules.begin()) item--; LOG("ERROR at PreUpdate, module %s", (*item)->name) }

	item = modules.begin();

	while(item != modules.end() && ret == true)
	{
		ret = (*item)->Update(dt);
		item++;
	}
	if (!ret && !input->quit) { if (item != modules.begin()) item--; LOG("ERROR at Update, module %s", (*item)->name) }

	item = modules.begin();

	while(item != modules.end() && ret == true)
	{
		ret = (*item)->PostUpdate(dt);
		item++;
	}
	if (!ret && !input->quit) { if (item != modules.begin()) item--; LOG("ERROR at PostUpdate, module %s", (*item)->name) }

	FinishUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::iterator item = modules.begin();

	while(item != modules.end() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}
	if (!ret) { if (item != modules.begin()) item--; LOG("ERROR at CleanUp, module %s", (*item)->name) }

	return ret;
}

void Application::AddModule(Module* mod)
{
	modules.push_back(mod);
}