#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ImGui/imgui.h"
#include "imGui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module("Input", start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
	quit = false;
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init(Config* config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
bool ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	mouse_motion_x = mouse_motion_y = 0;

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE) {
				keyboard[i] = KEY_DOWN;
			}
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) {
				keyboard[i] = KEY_UP;
			}
			else {
				keyboard[i] = KEY_IDLE;
			}
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[e.button.button - 1] = KEY_DOWN;
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_buttons[e.button.button - 1] = KEY_UP;
			break;

		case SDL_MOUSEMOTION:
			mouse_motion_x = e.motion.xrel;
			mouse_motion_y = e.motion.yrel;
			mouse_x = e.motion.x;
			mouse_y = e.motion.y;
			break;

		case SDL_MOUSEWHEEL:
			mouse_wheel = e.wheel.y;
			break;

			//case SDL_DROPFILE:
			//	Event ev(Event::file_dropped);
			//	ev.string.ptr = event.drop.file;
			//	App->BroadcastEvent(ev);
			//	SDL_free(event.drop.file);
			//	break;
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				App->renderer3D->OnResize(e.window.data1, e.window.data2);
		}
		}
	}

	if (quit == true) 
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP)
			return false;

		ImGui::OpenPopup("Quit");
		if (ImGui::BeginPopupModal("Quit", &quit, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Are you sure you want to Quit?");
			ImGui::NewLine();
			ImGui::Text("  ");
			ImGui::SameLine();

			if (ImGui::Button("Save"))
			{
				//Save
				ImGui::CloseCurrentPopup();

				LOG("SAVING APPLICATION AND EXITING");
				quit = true;
				return false;
			}
			ImGui::SameLine();
			ImGui::Text("");
			ImGui::SameLine();

			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();

				LOG("EXITING APPLICATION");
				quit = true;
				return false;

			}
			ImGui::SameLine();
			ImGui::Text("");
			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
				LOG("CANCEL EXIT");
				quit = false;
			}
			ImGui::EndPopup();
		}
	}

	return true;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}