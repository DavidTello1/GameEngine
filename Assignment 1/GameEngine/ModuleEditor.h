#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"


class ModuleEditor : public Module
{
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool CleanUp();

	void Draw() const;

private:
	// basic
	static bool is_show_demo;
	static bool is_draw_menu;

	//file
	static bool is_new;
	static bool is_load;
	static bool is_save;

	//window
	static bool is_show_console;
	static bool is_show_properties;
	static bool is_show_configuration;

	// shortcuts
	const char* new_docs = "Ctrl+N";
	const char* load_docs = "Ctrl+O";
	const char* save_docs = "Ctrl+S";

};

#endif
