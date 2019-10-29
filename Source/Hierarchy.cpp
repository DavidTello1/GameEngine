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
		sprintf_s(buffer, 120, "%s %ld", obj->GetName(), obj->GetUID());
		//strcpy_s(node->tmp_name, 120, node->name);

		if (obj->is_rename == false)
		{
			bool is_open = ImGui::TreeNodeEx(buffer, obj->hierarchy_flags);

			if (ImGui::BeginPopupContextItem(buffer)) // Options menu poped up when right clicking a node
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

			if (ImGui::IsItemClicked()) // if treenode is clicked, check whether it is a single or multi selection
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					obj->is_rename = true;
				}
				else {

					if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
					{
						//for (HierarchyNode* i : Hierarchy::selected_nodes) // Selected nodes has selected state, need to unselect, toggle is safe [panaderia de pan]
						//	i->ToggleSelection();

						App->scene->UnSelectAll(obj);
					}

					if (obj->ToggleSelection()) // Always need to toggle the state of selection of the node, getting its current state
					{
						//Hierarchy::selected_nodes.push_back(node);
					}
					/*else
					{
						SearchAndDeleteNode(obj, Hierarchy::selected_nodes);
					}*/
				}
			}
			if (is_open)
			{
				in_menu = true;
				if (obj->childs.size() > 0) // Node is open, need to draw childs if has childs
					DrawNodes(obj->childs);

				ImGui::TreePop();
			}
			else
				in_menu = false;

		}
		else // Rename
		{
			if (obj->is_selected == false)
			{
				obj->is_rename = false;
			}

			if (ImGui::InputText("##GameObject", buffer, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				obj->SetName(buffer);
				obj->is_rename = false;
			}
		}
	}
}
