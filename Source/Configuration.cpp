#include "Configuration.h"
#include "Application.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ModuleSceneBase.h"

#include "gpudetect/DeviceId.h"
#include "Devil/include/IL/il.h"

#include "mmgr/mmgr.h"

using namespace std;

// ---------------------------------------------------------
Configuration::Configuration() : Panel("Configuration"), fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	has_menubar = true;

	GetHardware(); //init hardware detection
	info_hw = GetHardwareInfo();
}

Configuration::~Configuration()
{
}

void Configuration::Draw()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			in_menu = true;

			if (ImGui::MenuItem("Load Config"))
				App->LoadPrefs();

			if (ImGui::MenuItem("Save Config"))
				App->SavePrefs();

			ImGui::EndMenu();
		}
		else
			in_menu = false;

		ImGui::EndMenuBar();
	}

	DrawApplication();

	DrawHardware();

	if (InitModuleDraw(App->window))
		DrawModuleWindow(App->window);

	if (InitModuleDraw(App->input))
		DrawModuleInput(App->input);

	//DrawTextures();
	
	if (InitModuleDraw(App->file_system))
		DrawModuleFileSystem(App->file_system);

	DrawScene();

	DrawMainCamera();
}

void Configuration::DrawScene()
{
	if (ImGui::CollapsingHeader("Scene"))
	{
		ImGui::Checkbox("Show all wireframe", &App->scene_base->show_all_wireframe);
		ImGui::ColorEdit3("Wireframe color", (float*)&App->scene_base->wireframe_color);
		ImGui::DragFloat("Wireframe width", &App->scene_base->wireframe_width, 0.1f, 0.1f, 5.0f);

		ImGui::Separator();

		ImGui::Checkbox("Show all bounding boxes", &App->scene_base->show_all_bounding_box);
		ImGui::ColorEdit3("Bounding box color", (float*)&App->scene_base->bounding_box_color);
		ImGui::DragFloat("Bounding box width", &App->scene_base->bounding_box_width, 0.1f,0.1f,5.0f);

		ImGui::Separator();

		ImGui::DragFloat("Plane length", &App->scene_base->plane_length, 1.0f, 0.0f, 5000.0f);
		ImGui::DragFloat("Axis length", &App->scene_base->axis_length, 1.0f, 0.0f, 5000.0f);
	}
}

void Configuration::DrawMainCamera()
{
	if (ImGui::CollapsingHeader("Main camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		
		ImGui::ColorEdit3("Background color", (float*)&viewport_camera->background);

		// Dummy floats
		float _fov  = viewport_camera->frustum.verticalFov * RADTODEG;
		float _near = viewport_camera->frustum.nearPlaneDistance;
		float _far  = viewport_camera->frustum.farPlaneDistance;

		if (ImGui::Checkbox("Perspective / Orthogonal", &viewport_camera->perspective))
		{
			if (viewport_camera->perspective)
				viewport_camera->frustum.type = FrustumType::PerspectiveFrustum;
			else
				viewport_camera->frustum.type = FrustumType::OrthographicFrustum;

			viewport_camera->update_projection = true;
		}

		//if (ImGui::DragFloat("Fov Y", &fov, 0.001f, 0.01f, PI)) //radians
		if (ImGui::DragFloat("Fov Y", &_fov, 0.1f, 0.1f, 180.0f,"%.1f"))
		{
			viewport_camera->SetFov(_fov, true);
		}
		if (ImGui::DragFloat("Z Near", &_near, 0.25f, 0.1f, _far))
		{
			viewport_camera->SetNearPlane(_near);
		}
		if (ImGui::DragFloat("Z Far",  &_far,	0.25f, _near, 5000.0f))
		{
			viewport_camera->SetFarPlane(_far);
		}
	}
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
		ImGui::Separator();

		// Software Versions
		if (ImGui::TreeNode("Software Versions"))
		{
			// SDL
			SDL_version compiled;
			SDL_VERSION(&compiled);

			ImGui::BulletText("SDL Version:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);

			// OpenGL
			ImGui::BulletText("OpenGL Version:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));

			// OpenGL
			ImGui::BulletText("Glew Version:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glewGetString(GLEW_VERSION));

			//ImGui
			ImGui::BulletText("ImGui Version:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", IMGUI_VERSION);

			// Devil
			ImGui::BulletText("DevIL Version:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", IL_VERSION);

			ImGui::TreePop();
		}
		ImGui::Separator();

		// FPS 
		if (ImGui::TreeNode("FPS"))
		{
			bool vsync = App->renderer3D->GetVSync();
			if (ImGui::Checkbox("Vertical Sync", &vsync))
				App->renderer3D->SetVSync(vsync);

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

			ImGui::TreePop();
		}
		ImGui::Separator();

		// Memory 
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
		if (ImGui::TreeNode("Memory"))
		{
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

			ImGui::TreePop();
		}
	}
}

void Configuration::DrawHardware()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{
		IMGUI_PRINT("CPUs:", "%u (Cache: %ukb)", info_hw.cpu_count, info_hw.l1_cachekb);
		IMGUI_PRINT("System RAM:", "%.1fGb", info_hw.ram_gb);
		IMGUI_PRINT("Caps:", "%s%s%s%s%s%s",
			info_hw.rdtsc ? "RDTSC," : "",
			info_hw.altivec ? "AltiVec," : "",
			info_hw.mmx ? "MMX," : "",
			info_hw.now3d ? "3DNow," : "",
			info_hw.sse ? "SSE," : "",
			info_hw.sse2 ? "SSE2," : "");
		IMGUI_PRINT("", "%s%s%s%s%s",
			info_hw.sse3 ? "SSE3," : "",
			info_hw.sse41 ? "SSE41," : "",
			info_hw.sse42 ? "SSE42," : "",
			info_hw.avx ? "AVX," : "",
			info_hw.avx2 ? "AVX2" : "");

		ImGui::Separator();
		IMGUI_PRINT("GPU:", "vendor %u device %u", info_hw.gpu_vendor, info_hw.gpu_device);
		IMGUI_PRINT("Brand:", info_hw.gpu_brand);
		IMGUI_PRINT("VRAM Budget:", "%.1f Mb", info_hw.vram_mb_budget);
		IMGUI_PRINT("VRAM Usage:", "%.1f Mb", info_hw.vram_mb_usage);
		IMGUI_PRINT("VRAM Available:", "%.1f Mb", info_hw.vram_mb_available);
		IMGUI_PRINT("VRAM Reserved:", "%.1f Mb", info_hw.vram_mb_reserved);
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

	float brightness = App->window->GetBrightness();
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
		App->window->SetBrightness(brightness);

	uint min_w, min_h, max_w, max_h;
	App->window->GetMaxMinSize(min_w, min_h, max_w, max_h);

	static int width = (int)App->window->GetWidth();
	static int height = (int)App->window->GetHeight();

	if (ImGui::DragInt("Width", &width, 1, min_w, max_w))
	{
		if ((uint)width > max_w)
			width = int(max_w);
		else if ((uint)width < min_w)
			width = int(min_w);

		App->window->SetWidth((uint)width);
	}

	if (ImGui::DragInt("Height", &height, 1, min_h, max_h))
	{
		if ((uint)height > max_h)
			height = max_h;
		else if ((uint)height < min_h)
			height = min_h;

		App->window->SetHeigth((uint)height);
	}

	ImGui::Text("Refresh rate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%u", App->window->GetRefreshRate());

	bool fullscreen = App->window->IsFullscreen();
	bool resizable = App->window->IsResizable();
	bool borderless = App->window->IsBorderless();
	bool full_desktop = App->window->IsFullscreenDesktop();

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->SetFullscreen(fullscreen);

	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->SetResizable(resizable);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Restart to apply");

	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->SetBorderless(borderless);

	ImGui::SameLine();
	if (ImGui::Checkbox("Full Desktop", &full_desktop))
		App->window->SetFullScreenDesktop(full_desktop);
}

void Configuration::DrawModuleInput(ModuleInput* module)
{
	int mouse_x, mouse_y;
	module->GetMousePosition(mouse_x, mouse_y);
	ImGui::Text("Mouse Position:");
	ImGui::SameLine();
	ImGui::Text("x");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_x);
	ImGui::SameLine();
	ImGui::Text("y");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_y);


	module->GetMouseMotion(mouse_x, mouse_y);
	ImGui::Text("Mouse Motion:");
	ImGui::SameLine();
	ImGui::Text("x");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_x);
	ImGui::SameLine();
	ImGui::Text("y");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_y);

	int wheel = module->GetMouseWheel();
	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", wheel);
}

//void Configuration::DrawTextures()
//{
//	if (ImGui::CollapsingHeader("Textures"))
//	{
//		if (App->scene->materials.empty())
//		{
//			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
//			ImGui::TextWrapped("No textures loaded");
//			ImGui::PopStyleColor();
//			ImGui::NewLine();
//			return;
//		}
//		char buffer[120];
//		for (uint i = 0; i < App->scene->materials.size(); i++)
//		{
//			sprintf_s(buffer, 120, "Texture %d", i);
//
//			if (ImGui::TreeNode(buffer))
//			{
//				ImGui::Text("Size: ");
//				ImGui::SameLine();
//				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i, %i", App->scene->materials[i]->width, App->scene->materials[i]->height);
//
//				ImGui::Text("Path: ");
//				ImGui::SameLine();
//				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
//				ImGui::TextWrapped("%s", App->scene->materials[i]->path);
//				ImGui::PopStyleColor();
//
//
//				ImGui::Text("Image:");
//				ImGui::Image((ImTextureID)App->scene->materials[i]->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
//				ImGui::TreePop();
//			}
//			ImGui::Separator();
//		}
//	}
//}

void Configuration::DrawModuleFileSystem(ModuleFileSystem* module)
{
	ImGui::Text("Base Path:");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::TextWrapped(module->GetBasePath());
	ImGui::PopStyleColor();

	ImGui::Text("Read Paths:");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::TextWrapped(module->GetReadPaths());
	ImGui::PopStyleColor();

	ImGui::Text("Write Path:");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::TextWrapped(module->GetWritePath());
	ImGui::PopStyleColor();
}


//---------------------------------
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

void Configuration::GetHardware()
{
	SDL_version version;
	SDL_GetVersion(&version);

	info_hw.ram_gb = (float)SDL_GetSystemRAM() / (1024.f);
	info_hw.cpu_count = SDL_GetCPUCount();
	info_hw.l1_cachekb = SDL_GetCPUCacheLineSize();
	info_hw.rdtsc = SDL_HasRDTSC() == SDL_TRUE;
	info_hw.altivec = SDL_HasAltiVec() == SDL_TRUE;
	info_hw.now3d = SDL_Has3DNow() == SDL_TRUE;
	info_hw.mmx = SDL_HasMMX() == SDL_TRUE;
	info_hw.sse = SDL_HasSSE() == SDL_TRUE;
	info_hw.sse2 = SDL_HasSSE2() == SDL_TRUE;
	info_hw.sse3 = SDL_HasSSE3() == SDL_TRUE;
	info_hw.sse41 = SDL_HasSSE41() == SDL_TRUE;
	info_hw.sse42 = SDL_HasSSE42() == SDL_TRUE;
	info_hw.avx = SDL_HasAVX() == SDL_TRUE;
	info_hw.avx2 = SDL_HasAVX2() == SDL_TRUE;

	uint vendor, device_id;
	std::wstring brand;
	unsigned __int64 video_mem_budget;
	unsigned __int64 video_mem_usage;
	unsigned __int64 video_mem_available;
	unsigned __int64 video_mem_reserved;

	if (getGraphicsDeviceInfo(&vendor, &device_id, &brand, &video_mem_budget, &video_mem_usage, &video_mem_available, &video_mem_reserved))
	{
		info_hw.gpu_vendor = vendor;
		info_hw.gpu_device = device_id;
		sprintf_s(info_hw.gpu_brand, 250, "%S", brand.c_str());
		info_hw.vram_mb_budget = float(video_mem_budget) / 1073741824.0f;
		info_hw.vram_mb_usage = float(video_mem_usage) / (1024.f * 1024.f * 1024.f);
		info_hw.vram_mb_available = float(video_mem_available) / (1024.f * 1024.f * 1024.f);
		info_hw.vram_mb_reserved = float(video_mem_reserved) / (1024.f * 1024.f * 1024.f);
	}
}

const hardware_info& Configuration::GetHardwareInfo() const
{
	unsigned __int64 video_mem_budget;
	unsigned __int64 video_mem_usage;
	unsigned __int64 video_mem_available;
	unsigned __int64 video_mem_reserved;

	if (getGraphicsDeviceInfo(nullptr, nullptr, nullptr, &video_mem_budget, &video_mem_usage, &video_mem_available, &video_mem_reserved))
	{
		info_hw.vram_mb_budget = float(video_mem_budget) / (1024.f * 1024.f);
		info_hw.vram_mb_usage = float(video_mem_usage) / (1024.f * 1024.f);
		info_hw.vram_mb_available = float(video_mem_available) / (1024.f * 1024.f);
		info_hw.vram_mb_reserved = float(video_mem_reserved) / (1024.f * 1024.f);
	}

	return info_hw;
}
