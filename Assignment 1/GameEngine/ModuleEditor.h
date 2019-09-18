#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"

class ModuleEditor : public Module
{

public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init() override;
	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool CleanUp() override;

public:
	bool draw_menu = true;
};

#endif // __MODULEEDITOR_H__
