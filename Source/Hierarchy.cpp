#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"

// Root node to start drawing
GameObject* Hierarchy::root_node; 
// List of all nodes (NOT used to draw)
std::vector<GameObject*> Hierarchy::nodes; 

// ---------------------------------------------------------
Hierarchy::Hierarchy() : Panel("Hierarchy")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	has_menubar = true;

	strcpy_s(scene_name, NAME_LENGTH, "GameObject");

	//root_node = new HierarchyNode(nullptr,nullptr);
	//nodes.push_back(root_node);
}

Hierarchy::~Hierarchy()
{
	for (uint i = 0; i < nodes.size(); i++) //nodes
	{
		DeleteNode(nodes[i]->node);
	}
}

HierarchyNode* Hierarchy::SearchById(const uint id)
{
	for (GameObject* n : nodes)
	{
		if (n->GetUID == id)
			return n->node;
	}
	return nullptr;
}

void Hierarchy::Draw()
{
	if (ImGui::BeginMenuBar())
	{
		//if (ImGui::MenuItem("Add Childs"))
		//{
		//	for (HierarchyNode* selected : selected_nodes)
		//		AddNode(nullptr, selected);
		//}

		if (ImGui::MenuItem("Delete") || App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
			DeleteSelected();

		ImGui::EndMenuBar();
	}

	ImGui::Text(scene_name);
	ImGui::Separator();

	DrawNodes(root_node->childs);
}

// Add a new dummy node, child of the passed node or root node if parent is missing
// Returns pointer to the added node
HierarchyNode* Hierarchy::AddNode(GameObject* object, GameObject* parent)
{
	if (object == nullptr) 
	{
		LOG("Cannot create node with empty gameobject", 'e');
		return nullptr;
	}

	HierarchyNode* n = new HierarchyNode(obj, parent);

	parent->childs.push_back(n);
	parent->flags &= ~HierarchyNode::leaf_flags;
	parent->flags |= HierarchyNode::base_flags;

	n->LogAction("Added child");
	parent->LogAction("to parent");

	n->ToggleSelection();
	// General list of all nodes, not used for printing
	nodes.push_back(n);

	/*if (parent->id != 0)
		ChildAdded(obj,parent);*/
	return n;

}

void Hierarchy::ChildAdded(GameObject* child, HierarchyNode* to)
{
	if (to->id == 0) return;

	// Checking if new child node is not an empty node
	if (child->is_valid_dimensions)
	{
		to->obj->min_vertex = child->min_vertex;
		to->obj->max_vertex = child->max_vertex;

		GetMinMaxVertex(to, &to->obj->min_vertex, &to->obj->max_vertex);

		to->obj->GenBoundingBox();

	}
}
void Hierarchy::DeleteNode(HierarchyNode* n)
{
	// if has no childs, delete the node
	if (n->childs.empty())
	{
		SearchAndDeleteNode(n, root_node->childs);
		SearchAndDeleteNode(n, nodes);

		if (n->parent)
		{
			SearchAndDeleteNode(n, n->parent->childs);

			if (n->parent->childs.empty())
			{
				n->parent->flags = HierarchyNode::leaf_flags;
			}
		}
		n->LogAction("Deleted");

		App->scene->DeleteGameobj(n->obj);
		delete(n);
	}
	else
	{
		// Delete childs before the actual node
		for (int i = n->childs.size()-1; i >= 0; i--)
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
	App->scene->selected_go.clear();
}

bool Hierarchy::SearchAndDeleteNode(HierarchyNode* n, std::vector<HierarchyNode*>& v)
{
	if (n == nullptr) return false;

	for (uint i = 0; i < v.size(); i++)
	{
		if (v[i] != nullptr && v[i]->id == n->id)
		{
			v.erase(v.begin() + i);
			return true;
		}
	}
	return false;
}

//-------------------------------------
void Hierarchy::DrawNodes(std::vector<HierarchyNode*>& v)
{
	static char buffer[120];

	for (HierarchyNode* node : v)
	{
		sprintf_s(buffer, 120, "%s %ld", node->name, node->id);
		strcpy_s(node->tmp_name, 120, node->name);

		if (node->is_rename == false)
		{
			bool is_open = ImGui::TreeNodeEx(buffer, node->flags);

			if (ImGui::BeginPopupContextItem(buffer)) // Options menu poped up when right clicking a node
			{
				in_menu = true;
				if (ImGui::MenuItem("Rename"))
					node->is_rename = true;

				else if (ImGui::MenuItem("Delete"))
					DeleteNode(node);

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
					node->is_rename = true;
				}
				else {

					if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
					{
						//for (HierarchyNode* i : Hierarchy::selected_nodes) // Selected nodes has selected state, need to unselect, toggle is safe [panaderia de pan]
						//	i->ToggleSelection();

						UnSelectAll(node);
					}

					if (node->ToggleSelection()) // Always need to toggle the state of selection of the node, getting its current state
					{
						Hierarchy::selected_nodes.push_back(node);
					}
					else
					{
						SearchAndDeleteNode(node, Hierarchy::selected_nodes);
					}
				}
			}
			if (is_open)
			{
				in_menu = true;
				if (node->childs.size() > 0) // Node is open, need to draw childs if has childs
					DrawNodes(node->childs);

				ImGui::TreePop();
			}
			else
				in_menu = false;

		}
		else // Rename
		{
			if (node->is_selected == false)
			{
				node->is_rename = false;
			}

			if (ImGui::InputText("##GameObject", node->tmp_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				node->obj->SetName(node->tmp_name);
				node->SetName(node->tmp_name);
				node->is_rename = false;
			}
		}
	}
}

void Hierarchy::UnSelectAll(HierarchyNode* keep_selected)
{
	if (keep_selected == nullptr) {

		for (HierarchyNode* i : Hierarchy::nodes)
		{
			if (i->is_selected)
			{
				i->ToggleSelection();
			}
		}
	}
	else {

		for (HierarchyNode* i : Hierarchy::nodes)
		{
			if (i->is_selected)
			{
				if (i->id == keep_selected->id) continue;
				i->ToggleSelection();
			}
		}
	}
	Hierarchy::selected_nodes.clear();
	App->scene->selected_go.clear();
}
void Hierarchy::SetSceneName(const char* name)
{
	LOG("Renaming scene from '%s' to '%s'", scene_name, name);
	strcpy_s(scene_name, name);
}

void Hierarchy::GetMinMaxVertex(const HierarchyNode* node,float3* abs_max, float3* abs_min)
{
	if (node->childs.empty())
	{
		GameObject* go = node->obj;
		if (!go->is_valid_dimensions) return;
		if (go->min_vertex.x < abs_min->x) abs_min->x = go->min_vertex.x;
		if (go->min_vertex.y < abs_min->y) abs_min->y = go->min_vertex.y;
		if (go->min_vertex.z < abs_min->z) abs_min->z = go->min_vertex.z;

		if (go->max_vertex.x > abs_max->x) abs_max->x = go->max_vertex.x;
		if (go->max_vertex.y > abs_max->y) abs_max->y = go->max_vertex.y;
		if (go->max_vertex.z > abs_max->z) abs_max->z = go->max_vertex.z;
	}
	else {
		for (HierarchyNode* child : node->childs)
		{
			GetMinMaxVertex(child, abs_max, abs_min);
		}

	}
}