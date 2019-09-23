#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "Console.h"
#include <string.h>
#include <algorithm>

using namespace std;

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "ModuleEditor";
}

// Destructor
ModuleEditor::~ModuleEditor()
{
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

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	return true;
}

bool ModuleEditor::Start()
{
	return true;
}


bool ModuleEditor::PreUpdate(float dt)
{
	// Start the frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return true;
}

bool ModuleEditor::Update(float dt)
{
	bool ret = true;
	static bool showdemo = true;
	static bool showconsole = false;
	bool draw_menu = true;

	// Main menu GUI
	if (draw_menu == true)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("New ...");
				ImGui::MenuItem("Load ...");
				ImGui::MenuItem("Save ...");

				if (ImGui::MenuItem("Quit", "ESC"))
					ret = false;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Console", NULL, &showconsole);
				ImGui::MenuItem("Configuration");
				ImGui::MenuItem("Properties");

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Show Demo"))
					showdemo = !showdemo;

				if (ImGui::MenuItem("Documentation"))
					ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/wiki", NULL, NULL, SW_SHOWNORMAL);


				if (ImGui::MenuItem("Latest Version"))
					ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/releases", NULL, NULL, SW_SHOWNORMAL);

				if (ImGui::MenuItem("Report a bug"))
					ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/issues", NULL, NULL, SW_SHOWNORMAL);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	// Show demo 
	if (showdemo)
	{
		ImGui::ShowDemoWindow(&showdemo);
		ImGui::ShowMetricsWindow();
	}
	if (showconsole) Console::ShowConsole(&showconsole);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		LOG("GEOMETRY LOG %d",25,'g')
	}
	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		char c[8] = "jdsig";
		LOG("DEBUG LOG %s", c,'d')
	}
	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
		LOG("VERBOSE LOG", 'v')
	}
	return ret;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Freeing editor gui");

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

