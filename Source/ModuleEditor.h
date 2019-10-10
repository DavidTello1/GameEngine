#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"
#include <vector>

class Panel;
class Configuration;
class Hierarchy;
class Console;
class Inspector;
class Viewport;

class ModuleEditor : public Module
{
private:
	std::vector<Panel*> panels;

public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	bool Start();
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool Update(float dt);
	bool CleanUp();

	void Draw();





	void CreateLink(const char* text, const char* url, bool bullet = false);
	Panel* GetPanel(const char* name);
	void LogFPS(float fps, float ms);

private:

	void DrawMenu(bool is_draw_menu, bool &is_new, bool &is_open, bool &is_save, bool &is_show_demo, bool &is_about);
	void DrawDemo(bool &is_show_demo);
	void DrawAbout(bool &is_about);
	void DrawPanels();

	//int GetWidth(Panel* panel) const { return tab_panels.width; }
	//int GetHeight(Panel* panel) const { return tab_panels[panel].height; }
	//int GetPosX(Panel* panel) const { return tab_panels[panel].pos_x; }
	//int GetPosY(Panel* panel) const { return tab_panels[panel].pos_y; }

public:
	Configuration* tab_configuration = nullptr;
	Hierarchy* tab_hierarchy = nullptr;
	Console* tab_console = nullptr;
	Inspector* tab_inspector = nullptr;
	Viewport* tab_viewport = nullptr;

private:
	Panel* panel_configuration;
	Panel* panel_hierarchy;
	Panel* panel_console;
	Panel* panel_inspector;
	Panel* panel_viewport;
};

#endif
