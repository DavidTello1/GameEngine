#include "Globals.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleScene.h"

#include "mmgr/mmgr.h"

ModuleGUI::ModuleGUI(bool start_enabled) : Module("GUI", start_enabled)
{
}

ModuleGUI::~ModuleGUI()
{
}

bool ModuleGUI::Init(Config* config)
{
	return true;
}

bool ModuleGUI::Start(Config* config)
{
	return true;
}

bool ModuleGUI::PreUpdate(float dt)
{
	for (GameObject* obj : ModuleScene::root_object->childs) //all objects in scene
	{
		if (obj->HasComponent(Component::Type::UI_Element)) //if has ui component
		{
			std::vector<Component*> ui = obj->GetAllComponentsOfType(Component::Type::UI_Element); //get all ui components
			if (!ui.empty())
			{
				for (uint i = 0; i < ui.size(); ++i) //update
				{
					UI_Element* element = (UI_Element*)ui[i];

					element->UpdateCollider(); //update colliders
					element->UpdateState(); //update state

					if (element->state == UI_Element::State::DRAGGING)
					{
						element->position2D = App->input->GetMousePosition();
					}
				}
			}
		}
	}
	return true;
}

bool ModuleGUI::PostUpdate(float dt)
{
	return true;
}

bool ModuleGUI::CleanUp()
{
	return true;
}

void ModuleGUI::Draw(ComponentCamera* camera)
{
	for (int i = 0; i < canvas.size(); i++)
	{
		if (canvas[i]->visible)
			canvas[i]->Draw(camera);
	}
}
