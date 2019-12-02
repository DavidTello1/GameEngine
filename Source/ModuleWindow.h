#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"

#include <string>
#include "SDL/include/SDL.h"

struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	void Save(Config* config) const override;
	void Load(Config* config) override;

	SDL_Window* GetWindow() const { return window; }
	uint GetWidth() const { return screen_width; }
	uint GetHeight() const { return screen_height; }
	void SetTitle(const char* title){ SDL_SetWindowTitle(window, title); }
	void SetWidth(uint width) { SDL_SetWindowSize(window, width, GetHeight()); screen_width = width; }
	void SetHeigth(uint height) { SDL_SetWindowSize(window, GetWidth(), height); screen_height = height;  }
	uint GetRefreshRate() const;
	void GetMaxMinSize(uint& min_width, uint& min_height, uint& max_width, uint& max_height) const;

	bool IsFullscreen() const { return fullscreen; }
	bool IsResizable() const { return resizable; }
	bool IsBorderless() const { return borderless; }
	bool IsFullscreenDesktop() const { return fullscreen_desktop; }
	float GetBrightness() const { return SDL_GetWindowBrightness(window); }
	//const char* GetIcon() const { return icon_file.c_str(); }

	void SetFullscreen(bool set);
	void SetResizable(bool set);
	void SetBorderless(bool set);
	void SetFullScreenDesktop(bool set);
	void SetBrightness(float set);
	//void SetIcon(const char* file);


private:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	uint screen_width = 1024;
	uint screen_height = 768;
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fullscreen_desktop = false;
	std::string icon_file;
};

#endif // __ModuleWindow_H__