#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Viewport.h"

#include "glew/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Imgui/imgui.h"
//#include <gl/GL.h>
//#include <gl/GLU.h>

#pragma comment(lib, "glew/libx86/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module("Renderer3D", start_enabled)
{

}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(Config* config)
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->GetWindow());
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	// Initialize glew
	GLenum error = glewInit();

	if (error != GLEW_OK)
	{
		LOG("Error initializing glew! %s\n", glewGetErrorString(error));
		ret = false;
	}

	if (ret == true)
	{
		// get version info
		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		//Use Vsync
		bool set_vsync = config->GetBool("Vertical Sync", false);
		vsync = !set_vsync; // force change
		SetVSync(set_vsync);

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		// Blend for transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	//// Projection matrix for
	OnResize(App->window->GetWidth(), App->window->GetHeight());

	return ret;
}

// PreUpdate: clear buffer
bool ModuleRenderer3D::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// RED
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());
		
	return true;
}

// PostUpdate present buffer to screen
bool ModuleRenderer3D::PostUpdate(float dt)
{
	//App->viewport->DrawScene();

	// Drawing of gui and panels(and viewport panel)

	

	App->editor->Draw();

	SDL_GL_SwapWindow(App->window->GetWindow());

	return true;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::Load(Config * config)
{
	SetVSync(config->GetBool("Vertical Sync", true));
	//viewport->Load(config);
}

void ModuleRenderer3D::Save(Config * config) const
{
	config->AddBool("Vertical Sync", GetVSync());
	//viewport->Save(config);
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::SetVSync(bool vsync)
{
	if (this->vsync != vsync)
	{
		this->vsync = vsync;
		if (SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}
}