#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Panel.h"
#include "Imgui/imgui.h"
#include <vector>

#define FPS_LOG_SIZE 100

#define IMGUI_PRINT(field, format, ...) \
	ImGui::Text(field); \
	ImGui::SameLine(); \
	ImGui::TextColored(ImVec4(1.0f, 1.0f,0.0f,1.0f), format, __VA_ARGS__)

class Module;
class ModuleEditor;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleFileSystem;

struct hardware_info 
{
	float ram_gb = 0.f;
	uint cpu_count = 0;
	uint l1_cachekb = 0;
	bool rdtsc = false;
	bool altivec = false;
	bool now3d = false;
	bool mmx = false;
	bool sse = false;
	bool sse2 = false;
	bool sse3 = false;
	bool sse41 = false;
	bool sse42 = false;
	bool avx = false;
	bool avx2 = false;
	uint gpu_vendor = 0;
	uint gpu_device = 0;
	char gpu_brand[250] = "";
	float vram_mb_budget = 0.f;
	float vram_mb_usage = 0.f;
	float vram_mb_available = 0.f;
	float vram_mb_reserved = 0.f;
};


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

	void DrawScene();

	void DrawMainCamera();

	bool InitModuleDraw(Module* module);
	void DrawApplication();
	void DrawHardware();
	void DrawModuleWindow(ModuleWindow * module);
	void DrawModuleInput(ModuleInput * module);
	void DrawResources();
	void DrawModuleFileSystem(ModuleFileSystem * module);

	void AddFPS(float fps, float ms);

	void GetHardware();
	const hardware_info& GetHardwareInfo() const;

private:
	ImGuiTextBuffer input_buf;
	bool need_scroll = false;
	std::vector<float> fps_log;
	std::vector<float> ms_log;

	mutable hardware_info info_hw;

};

#endif// __CONFIGURATION_H__
