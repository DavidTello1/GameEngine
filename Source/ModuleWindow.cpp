#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module("Window", start_enabled)
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		SDL_DisplayMode DM;
		SDL_GetDesktopDisplayMode(0, &DM);

		//Use OpenGL 3.2
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		App->SetAppName(TITLE);
		window = SDL_CreateWindow(App->GetAppName(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

		if(window == nullptr)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError(),'d');
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

bool ModuleWindow::Start()
{
	//SetDefaultIcon();

	//std::string icon_file = config->GetString("Icon", "");
	//if (icon_file.size() > 1)
	//	SetIcon(icon_file.c_str());

	//SetBrightness(config->GetFloat("Brightness", 1.0f));

	// Force to trigger a chain of events to refresh aspect ratios	
	SDL_SetWindowSize(window, width, height);

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != nullptr)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}