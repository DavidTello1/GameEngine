#include "Configuration.h"
#include "Application.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"

#include "mmgr/mmgr.h"

using namespace std;

// ---------------------------------------------------------
Configuration::Configuration() : Panel("Configuration"),
fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;
}

Configuration::~Configuration()
{
}

void Configuration::Draw()
{
	if (ImGui::BeginMenu("Options"))
	{
		ImGui::MenuItem("Set Defaults");
		if (ImGui::MenuItem("Load"))
		{ }
			//App->LoadPrefs();

		if (ImGui::MenuItem("Save"))
		{ }
			//App->SavePrefs();

		ImGui::EndMenu();
	}

	DrawApplication();

	if (InitModuleDraw(App->window))
		DrawModuleWindow(App->window);

	if (InitModuleDraw(App->input))
		DrawModuleInput(App->input);

	if (InitModuleDraw(App->renderer3D))
		DrawModuleRenderer(App->renderer3D);
	
	//if (InitModuleDraw(App->audio))
	//	DrawModuleAudio(App->audio);

	//if (InitModuleDraw(App->camera))
	//	DrawModuleCamera(App->camera);

	//if (InitModuleDraw(App->physics3D))
	//	DrawModulePhysics(App->physics3D);

	//if (InitModuleDraw(App->fs))
	//	DrawModuleFileSystem(App->fs);

	//if (InitModuleDraw(App->tex))
	//	DrawModuleTextures(App->tex);

	//if (InitModuleDraw(App->hw))
	//	DrawModuleHardware(App->hw);

	//if (InitModuleDraw(App->level))
	//	DrawModuleLevel(App->level);

	//if (InitModuleDraw(App->hints))
	//	DrawModuleHints(App->hints);
}

bool Configuration::InitModuleDraw(Module* module)
{
	bool ret = false;

	if (ImGui::CollapsingHeader(module->GetName()))
	{
		bool active = module->IsActive();
		if (ImGui::Checkbox("Active", &active))
			module->SetActive(active);
		ret = true;
	}

	return ret;
}

void Configuration::DrawApplication()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		static char app_name[120];
		strcpy_s(app_name, 120, App->GetAppName());
		if (ImGui::InputText("App Name", app_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->SetAppName(app_name);

		static char org_name[120];
		strcpy_s(org_name, 120, App->GetOrganizationName());
		if (ImGui::InputText("Organization", org_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->SetOrganizationName(org_name);

		int max_fps = App->GetFramerateLimit();
		if (ImGui::SliderInt("Max FPS", &max_fps, 0, 120))
			App->SetFramerateLimit(max_fps);

		ImGui::Text("Limit Framerate:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", App->GetFramerateLimit());

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

		// Memory --------------------
		sMStats stats = m_getMemoryStatistics();
		static int speed = 0;
		static vector<float> memory(100);
		if (++speed > 20)
		{
			speed = 0;
			if (memory.size() == 100)
			{
				for (uint i = 0; i < 100 - 1; ++i)
					memory[i] = memory[i + 1];

				memory[100 - 1] = (float)stats.totalReportedMemory;
			}
			else
				memory.push_back((float)stats.totalReportedMemory);
		}

		ImGui::PlotHistogram("##memory", &memory[0], memory.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

		ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);
	}
}

void Configuration::DrawModuleWindow(ModuleWindow* module)
{
	//static bool waiting_to_load_icon = false;

	//if (waiting_to_load_icon == true && App->editor->FileDialog("bmp"))
	//{
	//	const char* file = App->editor->CloseFileDialog();
	//	if (file != nullptr)
	//		App->window->SetIcon(file);
	//	waiting_to_load_icon = false;
	//}

	//ImGui::Text("Icon: ");
	//ImGui::SameLine();
	//if (ImGui::Selectable(App->window->GetIcon()))
	//	waiting_to_load_icon = true;

	//float brightness = App->window->GetBrightness();
	//if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
	//	App->window->SetBrightness(brightness);

	//uint w, h, min_w, min_h, max_w, max_h;
	//App->window->GetMaxMinSize(min_w, min_h, max_w, max_h);
	//w = App->window->GetWidth();
	//h = App->window->GetHeight();

	//if (ImGui::SliderInt("Width", (int*)&w, min_w, max_w))
	//	App->window->SetWidth(w);

	//if (ImGui::SliderInt("Height", (int*)&h, min_h, max_h))
	//	App->window->SetHeigth(h);

	//ImGui::Text("Refresh rate:");
	//ImGui::SameLine();
	//ImGui::TextColored(IMGUI_YELLOW, "%u", App->window->GetRefreshRate());

	//bool fullscreen = App->window->IsFullscreen();
	//bool resizable = App->window->IsResizable();
	//bool borderless = App->window->IsBorderless();
	//bool full_desktop = App->window->IsFullscreenDesktop();

	//if (ImGui::Checkbox("Fullscreen", &fullscreen))
	//	App->window->SetFullscreen(fullscreen);

	//ImGui::SameLine();
	//if (ImGui::Checkbox("Resizable", &resizable))
	//	App->window->SetResizable(resizable);
	//if (ImGui::IsItemHovered())
	//	ImGui::SetTooltip("Restart to apply");

	//if (ImGui::Checkbox("Borderless", &borderless))
	//	App->window->SetBorderless(borderless);

	//ImGui::SameLine();
	//if (ImGui::Checkbox("Full Desktop", &full_desktop))
	//	App->window->SetFullScreenDesktop(full_desktop);
}

void Configuration::DrawModuleInput(ModuleInput * module)
{
	//int mouse_x, mouse_y;
	//module->GetMousePosition(mouse_x, mouse_y);
	//ImGui::Text("Mouse Position:");
	//ImGui::SameLine();
	//ImGui::TextColored(IMGUI_YELLOW, "%i,%i", mouse_x, mouse_y);

	//module->GetMouseMotion(mouse_x, mouse_y);
	//ImGui::Text("Mouse Motion:");
	//ImGui::SameLine();
	//ImGui::TextColored(IMGUI_YELLOW, "%i,%i", mouse_x, mouse_y);

	//int wheel = module->GetMouseWheel();
	//ImGui::Text("Mouse Wheel:");
	//ImGui::SameLine();
	//ImGui::TextColored(IMGUI_YELLOW, "%i", wheel);

	//ImGui::Separator();

	//ImGui::BeginChild("Input Log");
	//ImGui::TextUnformatted(input_buf.begin());
	//if (need_scroll)
	//	ImGui::SetScrollHere(1.0f);
	//need_scroll = false;
	//ImGui::EndChild();
}

void Configuration::DrawModuleRenderer(ModuleRenderer3D* module)
{
	//ImGui::Text("Driver:");
	//ImGui::SameLine();
	//ImGui::TextColored(IMGUI_YELLOW, App->renderer3D->GetDriver());

	//bool vsync = App->renderer3D->GetVSync();
	//if (ImGui::Checkbox("Vertical Sync", &vsync))
	//	App->renderer3D->SetVSync(vsync);
}

void Configuration::AddFPS(float fps, float ms)
{
	static uint count = 0;
	if (count == FPS_LOG_SIZE)
	{
		for (uint i = 0; i < FPS_LOG_SIZE - 1; ++i)
		{
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}
	}
	else
		++count;

	fps_log[count - 1] = fps;
	ms_log[count - 1] = ms;
}
