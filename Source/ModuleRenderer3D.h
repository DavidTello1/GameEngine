#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(Config* config = nullptr);
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void Load(Config* config);
	void Save(Config* config) const;

	void SetVSync(bool vsync);
	bool GetVSync() const { return vsync; }
	const char* GetDriver() const { return SDL_GetCurrentVideoDriver(); }

public:
	SDL_GLContext context;

	bool vsync = false;
	bool in_game = false;
};