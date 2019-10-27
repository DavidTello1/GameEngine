#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"
#include "Panel.h"
#include <vector>

#define FILE_MAX 250

class Configuration;
class Hierarchy;
class Console;
class Inspector;
class Assets;
class Viewport;

class ModuleEditor : public Module
{
private:
	std::vector<Panel*> panels;

public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void Draw();

	void CreateLink(const char* text, const char* url, bool bullet = false);
	void LogFPS(float fps, float ms);
	 
	// Panels
	Panel* GetPanel(const char* name);
	int GetPanelWidth(Panel* panel) const { return panel->width; }
	int GetPanelHeight(Panel* panel) const { return panel->height; }
	int GetPanelPosX(Panel* panel) const { return panel->pos_x; }
	int GetPanelPosY(Panel* panel) const { return panel->pos_y; }
	bool GetPanelActive(Panel* panel) const { return panel->active; }

private:
	void DrawMenu(bool is_draw_menu, bool &is_new, bool &is_open, bool &is_save, bool &is_show_demo, bool &is_about, bool &is_import, bool &is_autoselect, bool &is_plane, bool &is_axis, bool &is_wireframe);
	void DrawDemo(bool &is_show_demo);
	void DrawAbout(bool &is_about);
	void DrawPanels(bool &is_auto_select);
	void ConfirmExit();
	void Shortcuts(bool &is_new, bool &is_open, bool &is_save);
	void DrawCreateMenu();

public:
	Configuration* tab_configuration = nullptr;
	Hierarchy* tab_hierarchy = nullptr;
	Console* tab_console = nullptr;
	Inspector* tab_inspector = nullptr;
	Viewport* tab_viewport = nullptr;
	Assets* tab_assets = nullptr;

	bool show_plane = true;
	bool show_axis = true;

private:
	bool close = false;
};

#endif
