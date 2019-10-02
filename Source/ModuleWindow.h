#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool Start();
	bool CleanUp();

	SDL_Window* GetWindow() const { return window; }
	uint GetWidth() const { return width; }
	uint GetHeight() const { return height; }
	void SetWidth(uint width) { SDL_SetWindowSize(window, width, GetHeight()); }
	void SetHeigth(uint height) { SDL_SetWindowSize(window, GetWidth(), height); }
	void SetTitle(const char* title){ SDL_SetWindowTitle(window, title); }

private:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	int width = SCREEN_WIDTH * SCREEN_SIZE;
	int height = SCREEN_HEIGHT * SCREEN_SIZE;
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
};

#endif // __ModuleWindow_H__