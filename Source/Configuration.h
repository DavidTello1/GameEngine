#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Panel.h"
#include "Imgui/imgui.h"
#include <vector>

#define FPS_LOG_SIZE 100

class Module;
class ModuleEditor;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
//class ModuleAudio;
//class ModuleEditorCamera;
//class ModulePhysics3D;
//class ModuleFileSystem;
//class ModuleTextures;
//class ModuleMeshes;
//class ModuleHardware;
//class ModuleLevelManager;
//class ModuleHints;
//class QuadtreeNode;

class Configuration : public Panel
{
public:
	static const uint default_width = 302;
	static const uint default_height = 862;
	static const uint default_pos_x = 978;
	static const uint default_pos_y = 19;

public:
	Configuration();
	virtual ~Configuration();

	void Draw();

	bool InitModuleDraw(Module* module);
	void DrawApplication();
	void DrawModuleWindow(ModuleWindow * module);
	void DrawModuleInput(ModuleInput * module);
	void DrawModuleRenderer(ModuleRenderer3D * module);
	//void DrawModuleAudio(ModuleAudio * module);
	//void DrawModuleCamera(ModuleEditorCamera * module);
	//void DrawModulePhysics(ModulePhysics3D * module);
	//void DrawModuleFileSystem(ModuleFileSystem * module);
	//void DrawModuleTextures(ModuleTextures * module);
	//void DrawModuleHardware(ModuleHardware * module);
	//void DrawModuleLevel(ModuleLevelManager * module);
	//void DrawModuleHints(ModuleHints * module);

	//void RecursiveDrawQuadtree(QuadtreeNode* node);
	//void AddInput(const char* entry);
	void AddFPS(float fps, float ms);

private:
	ImGuiTextBuffer input_buf;
	bool need_scroll = false;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
};

#endif// __CONFIGURATION_H__
