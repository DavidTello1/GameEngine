#pragma once
#include "Module.h"
#include "TextRenderer.hpp"


#define MAX_LIGHTS 8

class Color;

typedef void *SDL_GLContext;

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

public:
	glfreetype::font_data our_font;

	SDL_GLContext context;

	bool vsync = false;
	bool in_game = false;
};
	extern void glColorColorU(const Color & c);
	extern void glColorColorF(const Color & c);