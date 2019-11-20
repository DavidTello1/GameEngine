#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"

// ---------------------------------------------------------
Hierarchy::Hierarchy() : Panel("Hierarchy")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	has_menubar = true;

}

Hierarchy::~Hierarchy()
{}

void Hierarchy::Draw()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Delete") || App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
			App->scene->DeleteSelected();

		ImGui::EndMenuBar();
	}

	ImGui::Text(App->scene->GetName());
	ImGui::Separator();

	// Drawing scene gameobjects as nodes
	DrawNodes(ModuleScene::root_object->childs);
}

void Hierarchy::DrawNodes(std::vector<GameObject*>& v)
{

	for (GameObject* obj : v)
	{
		//sprintf_s(buffer, NAME_LENGTH, "%s ID: %ld", obj->GetName(), obj->GetUID());

		if (obj->is_rename == false)
		{
			bool is_open = ImGui::TreeNodeEx(obj->GetName(), obj->hierarchy_flags);

			if (ImGui::BeginPopupContextItem(obj->GetName())) // Options menu poped up when right clicking a node
			{
				in_menu = true;
				if (ImGui::MenuItem("Rename"))
					obj->is_rename = true;

				else if (ImGui::MenuItem("Delete"))
					App->scene->DeleteGameObject(obj);

				ImGui::EndPopup();
			}
			else
			{
				in_menu = false;
			}

			if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1)) // if treenode is clicked, check whether it is a single or multi selection
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					obj->is_rename = true;
				}
				else {
					// Single selection, clear selected nodes
					if (!ImGui::GetIO().KeyCtrl) 
					{
						App->scene->UnSelectAll(obj);
					}
					// Always need to toggle the state of selection of the node, getting its current state
					if (obj->ToggleSelection()) 
					{
						// Current state is selected
					}
					else
					{
						// Current state is unselected
					}
				}
			}
			if (is_open)
			{
				in_menu = true;
				// Node is open, need to draw childs if has childs
				if (obj->HasChilds()) 
					DrawNodes(obj->childs);

				ImGui::TreePop();
			}
			else
			{
				in_menu = false;
			}
		}
		else // Rename
		{
			if (obj->is_selected == false)
			{
				obj->is_rename = false;
			}

			sprintf_s(buffer, NAME_LENGTH, "%s", obj->GetName());
			if (ImGui::InputText("##GameObject", buffer, NAME_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				obj->SetName(buffer);
				obj->is_rename = false;
			}
		}
	}
}
