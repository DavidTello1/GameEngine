#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Viewport.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "Config.h"
#include "Color.h"


#include "glew/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Imgui/imgui.h"

#pragma comment( lib, "Freetype/include/freetype.lib" )

#pragma comment(lib, "glew/libx86/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include "mmgr/mmgr.h"

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
	LOG("Loading glew", 'd');
	
	/*glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);*/
	glEnable(GL_TEXTURE_2D);


	
	our_font.init("Library/Fonts/Wintersoul.ttf", 45 /* size */);

	return ret;
}

// PreUpdate: clear buffer
bool ModuleRenderer3D::PreUpdate(float dt)
{
	return true;
}

// PostUpdate present buffer to screen
bool ModuleRenderer3D::PostUpdate(float dt)
{
	//App->viewport->DrawScene();

	// Drawing of gui and panels(and viewport panel)
	//App->editor->Draw();

	glColor3f(1, 1, 1);

	glfreetype::print(our_font, 50 /* xpos */, 50 /* ypos */,
		"The quick brown fox blah blah blah");

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
	config->SetBool("Vertical Sync", GetVSync());
	//viewport->Save(config);
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

void glColorColorU(const Color &c)
{
	glColor3ub(c.r * 255, c.g * 255, c.b * 255);
}

void glColorColorF(const Color &c)
{
	glColor3f(c.r, c.g, c.b );

}

