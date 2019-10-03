#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"
#include <vector>

class Panel;
class Configuration;

class ModuleEditor : public Module
{
public:
	enum TabPanelEnum
	{
		TabPanelLeft = 0,
		TabPanelRight,
		TabPanelBottom,
		TabPanelCount
	};

	Configuration* tab_configuration = nullptr;

private:
	struct TabPanel
	{
		int width = 0;
		int height = 0;
		int pos_x = 0;
		int pos_y = 0;

		char* name = nullptr;
		std::vector<Panel*> panels;
	};

	TabPanel tab_panels[TabPanelCount];
	TabPanel visible_panels[TabPanelCount];

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
	bool ClosePanel(const char* name);
	void LogFPS(float fps, float ms);

	int GetWidth(TabPanelEnum panel) const { return tab_panels[panel].width; }
	int GetHeight(TabPanelEnum panel) const { return tab_panels[panel].height; }
	int GetPosX(TabPanelEnum panel) const { return tab_panels[panel].pos_x; }
	int GetPosY(TabPanelEnum panel) const { return tab_panels[panel].pos_y; }
};

#endif
