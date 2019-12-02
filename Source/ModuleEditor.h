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
class ComponentCamera;

enum Style {
	NEW,
	CLASSIC,
	DARK,
	LIGHT
};

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

	void Save(Config* config) const override;
	void Load(Config* config) override;

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
	Panel* GetPanelFocused() { return focused_panel; }

	void AddViewport(ComponentCamera* camera);

private:
	void DrawMenu();
	void DrawDemo();
	void DrawAbout();
	void DrawPanels();
	void ConfirmExit();
	void Shortcuts();

public:
	Panel* focused_panel = nullptr;

	Configuration* tab_configuration = nullptr;
	Hierarchy* tab_hierarchy = nullptr;
	Console* tab_console = nullptr;
	Inspector* tab_inspector = nullptr;
	Viewport* tab_viewport = nullptr;
	Assets* tab_assets = nullptr;

	//Bools
	static bool is_draw_menu ;
	static bool is_show_main_dockspace ;
	static bool is_show_demo ;
	static bool is_auto_select ;
	static bool is_about ;
	static bool is_new ;
	static bool is_open ;
	static bool is_save ;
	static bool is_import ;
	static bool is_plane ;
	static bool is_axis ;
	static bool is_wireframe ;
	static bool is_show_plane ;
	static bool is_show_axis ;

private:
	bool close = false;
	uint style = 0;
};

#endif
