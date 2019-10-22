#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"

std::vector<HierarchyNode*> Hierarchy::root_nodes; // List of nodes to start drawing
std::vector<HierarchyNode*> Hierarchy::nodes; // List of all nodes (NOT used to draw)
std::vector<HierarchyNode*> Hierarchy::selected_nodes; // Temporary list of selected nodes

Hierarchy::Hierarchy() : Panel("Hierarchy")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	scene_name = "Scene Name";
}

Hierarchy::~Hierarchy()
{
	for (int i = 0; i < nodes.size(); i++) //nodes
	{
		DeleteNode(nodes[i]);
	}
	nodes.clear();

	for (int i = 0; i < root_nodes.size(); i++) //root nodes
	{
		DeleteNode(root_nodes[i]);
	}
	root_nodes.clear();

	for (int i = 0; i < selected_nodes.size(); i++) //selected nodes
	{
		DeleteNode(selected_nodes[i]);
	}
	selected_nodes.clear();

}

void Hierarchy::Draw()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Add Free Node"))
			AddNode();

		if (ImGui::MenuItem("Add Childs"))
		{
			for (HierarchyNode* selected : selected_nodes)
				AddNode(selected);
		}

		if (ImGui::MenuItem("Delete"))
			DeleteSelected();

		ImGui::EndMenuBar();
	}

	ImGui::Text(scene_name);
	ImGui::Separator();

	static int prev_size = 0;
	if (!App->scene_intro->gameobjs.empty() && prev_size < App->scene_intro->gameobjs.size())
	{
		prev_size = App->scene_intro->gameobjs.size();
		for (int i = 0; i < App->scene_intro->gameobjs.size(); i++) // Add a node for each gameobject
		{
			AddNode(nullptr, App->scene_intro->gameobjs[i]);
		}
	}

	DrawNodes(root_nodes);
}

// Add a new dummy node, child of the passed node or root node if parent is nullptr
// Returns pointer to the added node
HierarchyNode* Hierarchy::AddNode(HierarchyNode* parent, GameObject* object)
{
	HierarchyNode* n = new HierarchyNode(parent, object);

	if (parent == nullptr)
	{
		root_nodes.push_back(n);
		n->LogAction("Added root");
	}
	else
	{
		parent->childs.push_back(n);
		parent->flags &= ~HierarchyNode::leaf_flags;
		parent->flags |= HierarchyNode::base_flags;
		//LOG("Added child '%s', id: %ld to parent '%s', id: %ld",n->name, n->id, parent->name, parent->id,'d');
		n->LogAction("Added child");
		parent->LogAction("to parent");

	}

	// General list of all nodes, not used for printing
	nodes.push_back(n);
	return n;

}

bool Hierarchy::SearchAndDeleteNode(HierarchyNode* n, std::vector<HierarchyNode*>& v)
{
	if (n == nullptr) return false;

	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] != nullptr && v[i]->id == n->id)
		{
			v.erase(v.begin()+i);
			return true;
		}
	}
	return false;
}

void Hierarchy::DeleteNode(HierarchyNode* n)
{
	// if has no childs, delete the node
	if (n->childs.empty())
	{
		SearchAndDeleteNode(n, root_nodes);
		SearchAndDeleteNode(n, nodes);

		n->childs.clear();

		if (n->parent)
		{
			SearchAndDeleteNode(n, n->parent->childs);

			if (n->parent->childs.empty())
			{
				n->parent->flags = HierarchyNode::leaf_flags;
			}
		}
		n->LogAction("Deleted");

		delete(n);
	}
	else
	{
		// Delete childs before the actual node
		for (int i = 0; i < n->childs.size(); i++)
		{
			DeleteNode(n->childs[i]);
		}

		n->childs.clear();
		DeleteNode(n);
	}
}

void Hierarchy::DeleteSelected()
{
	for (HierarchyNode* n : selected_nodes)
	{
		DeleteNode(n);
	}
	selected_nodes.clear();
}

// Problems with the last child and parent doing the same thing
void Hierarchy::DrawNodes(std::vector<HierarchyNode*>& node)
{
	static char buffer[120];
	static char name[120];
	std::vector<GameObject*> obj = App->scene_intro->gameobjs;

	for (int i = 0; i < obj.size(); i++)
	{
		sprintf_s(buffer, 120, "%s %d", obj[i]->GetName(), i + 1);
		strcpy_s(name, 120, obj[i]->GetName());

		ImVec2 pos = ImGui::GetCursorPos();
		static bool is_rename = false;
		bool is_open = false;
		
		if (obj[i]->is_rename) // Rename
		{
			ImGui::SetCursorPos(pos);
			if (ImGui::InputText("", name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				obj[i]->SetName(name);
				node[i]->SetName(name);
				obj[i]->is_rename = false;
			}
		}
		else if (ImGui::TreeNodeEx(buffer)) // Normal node
		{
			is_open = true;
		}
		
		if (ImGui::BeginPopupContextItem(buffer)) // Options menu poped up when right clicking a node
		{
			if (ImGui::MenuItem("Rename"))
			{
				obj[i]->is_rename = true;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Delete"))
			{
				App->scene_intro->DeleteGameobj(obj[i]);
			}

			ImGui::EndPopup();
		}

		//if (ImGui::IsItemClicked()) // If treenode is clicked, check whether it is a single or multi selection
		//{
		//	if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
		//	{
		//		for (HierarchyNode* i : Hierarchy::selected_nodes)
		//			i->ToggleSelection(); // Selected nodes has selected state, need to unselect, toggle is safe [panaderia de pan]
		//		
		//		Hierarchy::selected_nodes.clear();
		//		LOG("Single selection", 'v');
		//	}
		//	else
		//		LOG("Multi Selection", 'v');

		//	if (node[i]->ToggleSelection()) // Always need to toggle the state of selection of the node, getting its current state
		//		Hierarchy::selected_nodes.push_back(node[i]);
		//	else
		//		SearchAndDeleteNode(node[i], Hierarchy::selected_nodes);
		//}

		if (is_open)
		{
			//if (node[i]->childs.size() > 0) // Node is open, need to draw childs if has childs
			//	DrawNodes(node[i]->childs);

			ImGui::TreePop();
		}
	}
}

void Hierarchy::SetSceneName(const char* name)
{
	LOG("Renaming scene from '%s' to '%s'", scene_name, name);
	scene_name = name;
}
