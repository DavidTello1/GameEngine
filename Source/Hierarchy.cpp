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

	/*strcpy_s(scene_name, NAME_LENGTH, "Scene Name");

	root_obj = new GameObject("Root");
	root_obj->uid = 0;

	nodes.push_back(root_obj);*/
}

Hierarchy::~Hierarchy()
{
	//for (uint i = 0; i < nodes.size(); i++) //nodes
	//{
	//	DeleteNode(nodes[i]);
	//}
}

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
	DrawNodes(App->scene->gameObjects);
}

// Add a new dummy node, child of the passed node or root node if parent is missing
// Returns pointer to the added node
//void Hierarchy::AddNode(GameObject* object, GameObject* parent)
//{
//	if (object == nullptr) 
//	{
//		LOG("Cannot create node with empty gameobject", 'e');
//		return;
//	}
//	if (parent == nullptr) parent = root_obj;
//
//	//HierarchyNode* n = new HierarchyNode(object, parent);
//
//	parent->childs.push_back(object);
//
//	object->Select();
//
//	// General list of all nodes, not used for printing
//	nodes.push_back(object);
//
//	object->LogAction("Added child");
//	parent->LogAction("to parent");
//
//	if (parent != root_obj)
//		parent->ChildAdded(object);
//
//}

// MUST NOT BE CALLED BY ANYONE BUT ~GameObject
//void Hierarchy::DeleteNode(GameObject* obj)
//{
//
//	SearchAndDeleteNode(obj, root_obj->childs);
//	SearchAndDeleteNode(obj, nodes);
//
//	obj->LogAction("Deleted");
//}

//void Hierarchy::DeleteNode(GameObject* obj)
//{
//	// if has no childs, delete the node
//	if (obj->childs.empty())
//	{
//		SearchAndDeleteNode(obj, root_obj->childs);
//		SearchAndDeleteNode(obj, nodes);
//
//		if (obj->parent)	SearchAndDeleteNode(obj, obj->parent->childs);
//
//		obj->LogAction("Deleted");
//
//		//App->scene->DeleteGameobj(obj);
//		//delete(obj);
//	}
//	else
//	{
//		// Delete childs before the actual node
//		for (int i = obj->childs.size()-1; i >= 0; i--)
//		{
//			DeleteNode(obj->childs[i]);
//		}
//
//		obj->childs.clear();
//		DeleteNode(obj);
//	}
//}

//void Hierarchy::DeleteSelected()
//{
//	for (GameObject* obj : nodes)
//	{
//		if (obj->is_selected)
//		{
//			DeleteNode(obj);
//			App->scene->DeleteGameObject(obj);
//		}
//	}
//
//	App->scene->selected_go.clear();
//}

// returns true if deleted, false otherwise
//bool Hierarchy::SearchAndDeleteNode(GameObject* n, std::vector<GameObject*>& v)
//{
//	if (n == nullptr | v.empty()) return false;
//
//	for (int i = 0; i < v.size(); i++)
//	{
//		if (v[i] != nullptr && v[i]->GetUID() == n->GetUID())
//		{
//			v.erase(v.begin() + i);
//			return true;
//		}
//	}
//	return false;
//}

//-------------------------------------
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

//void Hierarchy::UnSelectAll(GameObject* keep_selected)
//{
//	if (keep_selected == nullptr) {
//
//		for (GameObject* i : nodes)
//		{
//			i->UnSelect();
//		}
//	}
//	else {
//
//		for (GameObject* i : nodes)
//		{
//			if (i->GetUID() == keep_selected->GetUID()) 
//				continue;
//			i->UnSelect();
//		}
//	}
//
//	App->scene->selected_go.clear();
//}
