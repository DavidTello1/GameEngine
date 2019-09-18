#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"

#include <string.h>
#include <algorithm>

using namespace std;

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui/examples/imgui_impl_opengl3.h"


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
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	return true;
}

bool ModuleEditor::Start()
{
	//conf->active = config->GetBool("ConfActive", true);
	//props->active = config->GetBool("PropsActive", true);

	//OnResize(App->window->GetWidth(), App->window->GetHeight());

	return true;
}


bool ModuleEditor::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return true;
}

bool ModuleEditor::Update(float dt)
{
	bool ret = true;
	static bool showcase = true;

	// Main menu GUI
	if (draw_menu == true)
	{
		if (ImGui::BeginMainMenuBar())
		{
			bool selected = false;
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("New ...");
				ImGui::MenuItem("Load ...");

				if (ImGui::MenuItem("Quit", "ESC"))
					ret = false;

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	// Show showcase ? 
	if (showcase)
	{
		ImGui::ShowDemoWindow();
		ImGui::ShowMetricsWindow();
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