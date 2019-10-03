#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "Console.h"
#include "Hierarchy.h"
#include "Configuration.h"

#include <string.h>
#include <algorithm>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

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
bool ModuleEditor::Init()
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
	//ImGui::StyleColorsClassic();

	//Hierarchy::Init();
	return true;
}

bool ModuleEditor::Start()
{
	// Create panels
	panels.push_back(tab_configuration = new Configuration());


	// Init panels
	panel_configuration = GetPanel("Configuration");

	return true;
}


bool ModuleEditor::PreUpdate(float dt)
{
	// Start the frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();

	return true;
}

bool ModuleEditor::Update(float dt)
{
	bool ret = true;

	bool is_draw_menu = true;
	static bool is_show_main_dockspace = true;
	static bool is_show_demo = false;
	static bool is_about = false;

	static bool is_show_hierarchy = true;
	static bool is_show_console = false;
	static bool is_show_properties = false;

	static bool is_new = false;
	static bool is_open = false;
	static bool is_save = false;


	ShowExampleAppDockSpace(&is_show_main_dockspace);

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

			if (ImGui::BeginMenu("View")) //view
			{
				ImGui::MenuItem("Hierarchy", NULL, &is_show_hierarchy);
				ImGui::MenuItem("Console", NULL, &is_show_console);
				ImGui::MenuItem("Configuration", NULL, &panel_configuration->active);
				ImGui::MenuItem("Properties", NULL, &is_show_properties);

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


	if (is_show_demo) //show demo
	{
		ImGui::ShowDemoWindow(&is_show_demo);
		ImGui::ShowMetricsWindow();
	}

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
			CreateLink("SDL 2.0.10", "", true);
			CreateLink("Glew 2.0.0", "", true);
			CreateLink("ImGui 1.73", "", true);
			CreateLink("OpenGL 3.1", "", true); ImGui::NewLine();

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

	if (is_show_console) //console
		Console::ShowConsole(&is_show_console);
	//tab_panels[TabPanelBottom].panels.push_back(tab_console = new Console());

	if (is_show_hierarchy) //hierarchy
		Hierarchy::ShowHierarchy(&is_show_hierarchy);
	//tab_panels[TabPanelLeft].panels.push_back(tab_hierarchy = new Hierarchy());

	if (is_show_properties) //properties
	{
	}

	// Draw all active panels
	for (vector<Panel*>::const_iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->IsActive())
		{
			if (ImGui::Begin((*it)->GetName(), &(*it)->active))
			{
				(*it)->Draw();
				ImGui::End();
			}
		}
	}

	if (is_new) //new
	{
		//...
		is_new = false;
	}

	if (is_open) //open
	{
		//....
		is_open = false;
	}

	if (is_save) //save
	{
		//...
		is_save = false;
	}


	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		LOG("GEOMETRY LOG %d", 25, 'g')
	}
	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		char c[8] = "jdsig";
		LOG("DEBUG LOG %s", c, 'd')
	}
	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
		LOG("VERBOSE LOG", 'v')
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

	return ret;
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

void ModuleEditor::Draw() const
{
	// Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
