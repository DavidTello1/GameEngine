#include "Globals.h"
#include "Application.h"
#include "Config.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "Console.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Configuration.h"
#include "Assets.h"
#include "Viewport.h"

#include <string.h>
#include <algorithm>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "mmgr/mmgr.h"

using namespace std;

ModuleEditor::ModuleEditor(bool start_enabled) : Module("ModuleEditor", start_enabled)
{
}

// Destructor
ModuleEditor::~ModuleEditor()
{
}

static void ShowExampleAppDockSpace(bool* p_open);
void ShowExampleAppDockSpace(bool* p_open)
{
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();

	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);


	ImGui::End();
}

// Called before render is available
bool ModuleEditor::Init(Config* config)
{
	LOG("Init editor gui with imgui lib version %s", ImGui::GetVersion());

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsNew();

	return true;
}

bool ModuleEditor::Start(Config* config)
{
	// Create panels
	panels.push_back(tab_hierarchy = new Hierarchy());
	panels.push_back(tab_configuration = new Configuration());
	panels.push_back(tab_inspector = new Inspector());
	panels.push_back(tab_console = new Console());
	panels.push_back(tab_assets = new Assets());
	panels.push_back(tab_viewport = new Viewport());

	tab_viewport->Generate();

	return true;
}

bool ModuleEditor::PreUpdate(float dt)
{
	// Start the frame
	// Call preupdate viewport
	tab_viewport->PreUpdate();

	return true;
}

bool ModuleEditor::Update(float dt)
{
	return true;
}

bool ModuleEditor::PostUpdate(float dt)
{
	// end the frame
	// Call postupdate viewport
	tab_viewport->PostUpdate();

	if (close)
		return false;

	return true;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Freeing editor gui");

	for (vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		RELEASE(*it);
	}
	panels.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

// Drawing of the FULL gui
// First gets drawn the Menus, then panels
void ModuleEditor::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();

	bool ret = true;

	bool is_draw_menu = true;
	static bool is_show_main_dockspace = true;
	static bool is_show_demo = false;
	static bool is_about = false;

	static bool is_new = false;
	static bool is_open = false;
	static bool is_save = false;

	static bool is_import = false;

	ShowExampleAppDockSpace(&is_show_main_dockspace);
	DrawMenu(is_draw_menu, is_new, is_open, is_save, is_show_demo, is_about, is_import);
	DrawDemo(is_show_demo);
	DrawAbout(is_about);
	DrawPanels();

	if (file_dialog == opened)
		LoadFile((file_dialog_filter.length() > 0) ? file_dialog_filter.c_str() : nullptr);
	else
		in_modal = false;

	if (is_new)
	{
		//...
		is_new = false;
	}

	if (is_open)
	{
		//...
		is_open = false;
	}

	if (is_save) //save
	{
		App->file_system->Save(SETTINGS_FOLDER "Engine.log", App->GetLog().c_str(), App->GetLog().size());
		App->SavePrefs();
		is_save = false;
	}

	if (is_import)
	{
		//...
		is_import = false;
	}

	// --- SHORTCUTS -----------------
	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) ||
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN))
	{
		is_new = true;
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) ||
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN))
	{
		is_open = true;
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) ||
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN))
	{
		is_save = true;
	}

	if ((App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP))
	{
		App->input->quit = true;
	}

	if (App->input->quit == true)
	{
		ConfirmExit();
	}

	// Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::DrawMenu(bool is_draw_menu, bool &is_new, bool &is_open, bool &is_save, bool &is_show_demo, bool &is_about, bool &is_import)
{
	bool ret = true;

	if (is_draw_menu == true) // Main menu GUI
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File")) //file
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					is_new = true;

				if (ImGui::MenuItem("Open", "Ctrl+O"))
					is_open = true;

				if (ImGui::MenuItem("Save", "Ctrl+S"))
					is_save = true;

				if (ImGui::MenuItem("Quit", "ESC"))
					App->input->quit = true;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) //edit
			{
				if (ImGui::MenuItem("Import"))
					is_import = true;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) //view
			{
				ImGui::MenuItem("Hierarchy", NULL, &GetPanel("Hierarchy")->active);
				ImGui::MenuItem("Configuration", NULL, &GetPanel("Configuration")->active);
				ImGui::MenuItem("Inspector", NULL, &GetPanel("Inspector")->active);
				ImGui::MenuItem("Console", NULL, &GetPanel("Console")->active);
				ImGui::MenuItem("Assets", NULL, &GetPanel("Assets")->active);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) //help
			{
				ImGui::MenuItem("Show Demo", NULL, &is_show_demo);

				if (ImGui::MenuItem("Documentation"))
					ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/wiki", NULL, NULL, SW_SHOWNORMAL);


				if (ImGui::MenuItem("Latest Version"))
					ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/releases", NULL, NULL, SW_SHOWNORMAL);

				if (ImGui::MenuItem("Report a bug"))
					ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/issues", NULL, NULL, SW_SHOWNORMAL);

				if (ImGui::MenuItem("About"))
					is_about = true;

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}

void ModuleEditor::DrawDemo(bool &is_show_demo)
{
	if (is_show_demo) //show demo
	{
		ImGui::ShowDemoWindow(&is_show_demo);
		ImGui::ShowMetricsWindow();
	}
}

void ModuleEditor::DrawAbout(bool &is_about)
{
	if (is_about) //about
	{
		ImGui::OpenPopup("About");
		if (ImGui::BeginPopupModal("About"))
		{
			ImGui::Text("Davos Game Engine");
			ImGui::Text("Description");
			ImGui::Text("By");
			ImGui::SameLine();
			CreateLink("Oscar Pons", "https://github.com/ponspack9");
			ImGui::SameLine();
			ImGui::Text("&");
			ImGui::SameLine();
			CreateLink("David Tello", "https://github.com/DavidTello1");
			ImGui::NewLine();

			ImGui::Text("3rd Party Libraries used:");
			CreateLink("SDL", "http://www.libsdl.org/index.php", true);
			CreateLink("OpenGL", "https://opengl.org/", true);
			CreateLink("Glew", "http://glew.sourceforge.net/", true);
			CreateLink("ImGui", "https://github.com/ocornut/imgui", true);
			CreateLink("DevIL", "http://openil.sourceforge.net/", true);
			ImGui::NewLine();

			ImGui::Text("License:");
			ImGui::Text("MIT License");
			ImGui::Text("Copyright 2019. Oscar Pons and David Tello");
			ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files(the 'Software'), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions :");
			ImGui::Text("The above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.");
			ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.");
			ImGui::NewLine();

			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
				is_about = false;
			}
			ImGui::EndPopup();
		}
	}
}

void ModuleEditor::DrawPanels()
{
	for (vector<Panel*>::const_iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->IsActive())
		{
			if (ImGui::Begin((*it)->GetName(), &(*it)->active))
			{
				(*it)->Draw();
			}
			ImGui::End();
		}
	}
}

void ModuleEditor::ConfirmExit()
{
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP)
		close = true;

	ImGui::OpenPopup("Quit");
	if (ImGui::BeginPopupModal("Quit", &App->input->quit, ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("Are you sure you want to Quit?");
		ImGui::NewLine();
		//ImGui::Text("  ");
		//ImGui::SameLine();

		//if (ImGui::Button("Save"))
		//{
		//	//Save
		//	ImGui::CloseCurrentPopup();

		//	LOG("SAVING APPLICATION AND EXITING");
		//	quit = true;
		//	return false;
		//}
		//ImGui::SameLine();
		//ImGui::Text("");
		//ImGui::SameLine();

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();

			LOG("EXITING APPLICATION");
			close = true;
		}
		ImGui::SameLine();
		ImGui::Text("");
		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			LOG("CANCEL EXIT");
			close = false;
		}
		ImGui::EndPopup();
	}
}

void ModuleEditor::CreateLink(const char* text, const char* url, bool bullet)
{
	ImVec2 size = ImGui::CalcTextSize(text);
	ImVec2 pos = ImGui::GetCursorPos();
	ImVec4 color;

	if (bullet)
	{
		ImGui::BulletText(text);
		pos.x += 21;
	}
	else
		ImGui::Text(text);
	ImGui::SameLine();

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	}
	else
		color = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);

	ImGui::SetCursorPos(pos);
	ImGui::TextColored(color, text);
	ImGui::SameLine();

	ImGui::SetCursorPos(pos);

	if (ImGui::InvisibleButton(text, ImVec2(size)))
	{
		ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
	}
}

bool ModuleEditor::FileDialog(const char * extension, const char* from_folder)
{
	bool ret = true;

	switch (file_dialog)
	{
	case closed:
		selected_file[0] = '\0';
		file_dialog_filter = (extension) ? extension : "";
		file_dialog_origin = (from_folder) ? from_folder : "";
		file_dialog = opened;
	case opened:
		ret = false;
		break;
	}

	return ret;
}

const char * ModuleEditor::CloseFileDialog()
{
	if (file_dialog == ready_to_close)
	{
		file_dialog = closed;
		return selected_file[0] ? selected_file : nullptr;
	}
	return nullptr;
}

void ModuleEditor::LoadFile(const char* filter_extension, const char* from_dir)
{
	ImGui::OpenPopup("Load File");
	if (ImGui::BeginPopupModal("Load File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		in_modal = true;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
		ImGui::BeginChild("File Browser", ImVec2(0, 300), true);
		DrawDirectoryRecursive(from_dir, filter_extension);
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::PushItemWidth(250.f);
		if (ImGui::InputText("##file_selector", selected_file, FILE_MAX, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			file_dialog = ready_to_close;

		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Ok", ImVec2(50, 20)))
			file_dialog = ready_to_close;
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			file_dialog = ready_to_close;
			selected_file[0] = '\0';
		}

		ImGui::EndPopup();
	}
	else
	{
		in_modal = false;
	}
}

void ModuleEditor::DrawDirectoryRecursive(const char* directory, const char* filter_extension)
{
	vector<string> files;
	vector<string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	App->file_system->DiscoverFiles(dir.c_str(), files, dirs);

	for (vector<string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str()))
		{
			DrawDirectoryRecursive((dir + (*it)).c_str(), filter_extension);
			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());

	for (vector<string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const string& str = *it;

		bool ok = true;

		if (filter_extension && str.substr(str.find_last_of(".") + 1) != filter_extension)
			ok = false;

		if (ok && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::IsItemClicked()) {
				sprintf_s(selected_file, FILE_MAX, "%s%s", dir.c_str(), str.c_str());

				if (ImGui::IsMouseDoubleClicked(0))
					file_dialog = ready_to_close;
			}

			ImGui::TreePop();
		}
	}
}

Panel* ModuleEditor::GetPanel(const char* name)
{
	for (vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			return (*it);
		}
	}
	return nullptr;
}

void ModuleEditor::LogFPS(float fps, float ms)
{
	if (tab_configuration != nullptr)
		tab_configuration->AddFPS(fps, ms);
}
