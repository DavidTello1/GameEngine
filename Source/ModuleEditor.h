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
	void LogFPS(float fps, float ms);
	 
	// File System
	bool FileDialog(const char* extension = nullptr, const char* from_folder = nullptr);
	const char* CloseFileDialog();

	// Panels
	int GetWidth(Panel* panel) const { return panel->width; }
	int GetHeight(Panel* panel) const { return panel->height; }
	int GetPosX(Panel* panel) const { return panel->pos_x; }
	int GetPosY(Panel* panel) const { return panel->pos_y; }

private:
	Panel* GetPanel(const char* name);
	void LoadFile(const char* filter_extension = nullptr, const char* from_dir = nullptr);
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);

public:
	Configuration* tab_configuration = nullptr;
	Hierarchy* tab_hierarchy = nullptr;
	Console* tab_console = nullptr;
	Inspector* tab_inspector = nullptr;

private:
	enum
	{
		closed,
		opened,
		ready_to_close
	} file_dialog = closed;

	std::string file_dialog_filter;
	std::string file_dialog_origin;

	//bool capture_mouse = false;
	//bool capture_keyboard = false;
	bool in_modal = false;
	char selected_file[FILE_MAX];
	bool draw_menu = true;

};

#endif
