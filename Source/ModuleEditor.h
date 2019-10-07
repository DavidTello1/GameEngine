#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"
#include <vector>

class Panel;
class Configuration;
class Hierarchy;
class Console;
class Inspector;

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
	bool Update(float dt);
	bool CleanUp();

	void Draw() const;
	void CreateLink(const char* text, const char* url, bool bullet = false);
	Panel* GetPanel(const char* name);
	void LogFPS(float fps, float ms);

	//int GetWidth(Panel* panel) const { return tab_panels.width; }
	//int GetHeight(Panel* panel) const { return tab_panels[panel].height; }
	//int GetPosX(Panel* panel) const { return tab_panels[panel].pos_x; }
	//int GetPosY(Panel* panel) const { return tab_panels[panel].pos_y; }

public:
	Configuration* tab_configuration = nullptr;
	Hierarchy* tab_hierarchy = nullptr;
	Console* tab_console = nullptr;
	Inspector* tab_inspector = nullptr;

private:
	Panel* panel_configuration;
	Panel* panel_hierarchy;
	Panel* panel_console;
	Panel* panel_inspector;
};

#endif
