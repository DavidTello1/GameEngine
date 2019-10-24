#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"

HierarchyNode* Hierarchy::root_node; // Root node to start drawing
std::vector<HierarchyNode*> Hierarchy::nodes; // List of all nodes (NOT used to draw)
std::vector<HierarchyNode*> Hierarchy::selected_nodes; // Temporary list of selected nodes

// ---------------------------------------------------------
Hierarchy::Hierarchy() : Panel("Hierarchy")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	scene_name = "Scene Name";

	root_node = new HierarchyNode(nullptr,nullptr);
	nodes.push_back(root_node);
}

Hierarchy::~Hierarchy()
{
	for (uint i = 0; i < nodes.size(); i++) //nodes
	{
		DeleteNode(nodes[i]);
	}
	nodes.clear();

	//for (uint i = 0; i < root_nodes.size(); i++) //root nodes
	//{
	//	DeleteNode(root_nodes[i]);
	//}
	//root_nodes.clear();

	selected_nodes.clear();
	//for (uint i = 0; i < selected_nodes.size(); i++) //selected nodes
	//{
	//	DeleteNode(selected_nodes[i]);
	//}
}

HierarchyNode* Hierarchy::SearchById(const uint id)
{
	for (HierarchyNode* n : nodes)
	{
		if (n->id == id)
			return n;
	}
	return nullptr;
}

void Hierarchy::Draw()
{
	if (ImGui::BeginMenuBar())
	{
		DrawCreateMenu();

		if (ImGui::MenuItem("Add Childs"))
		{
			for (HierarchyNode* selected : selected_nodes)
				AddNode(nullptr, selected);
		}

		if (ImGui::MenuItem("Delete") || App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
			DeleteSelected();

		ImGui::EndMenuBar();
	}

	ImGui::Text(scene_name);
	ImGui::Separator();

	DrawNodes(root_node->childs);
}

void Hierarchy::DrawCreateMenu()
{
	in_menu = false;

	if (ImGui::BeginMenu("Create"))
	{
		in_menu = true;
		if (ImGui::MenuItem("Empty"))
		{
			App->scene->CreateGameObj();
		}
		ImGui::Separator();
		if (ImGui::BeginMenu("Basic shapes"))
		{
			if (ImGui::MenuItem("Cylinder"))
			{
				App->scene->CreateShape(CYLINDER, 9, 9);
			}
			if (ImGui::MenuItem("Cone"))
			{
				App->scene->CreateShape(CONE, 9, 9);
			}
			if (ImGui::MenuItem("Sphere"))
			{
				App->scene->CreateShape(SPHERE, 9, 9);
			}
			if (ImGui::MenuItem("Plane"))
			{
				App->scene->CreateShape(PLANE, 9, 9);
			}
			if (ImGui::MenuItem("Cube"))
			{
				App->scene->CreateShape(CUBE, 9, 9);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Extended shapes"))
		{
			if (ImGui::MenuItem("Torus"))
			{
				App->scene->CreateShape(TORUS, 9, 9);
			}
			if (ImGui::MenuItem("Bottle"))
			{
				App->scene->CreateShape(BOTTLE, 9, 9);
			}
			if (ImGui::MenuItem("Knot"))
			{
				App->scene->CreateShape(KNOT, 9, 9);
			}
			if (ImGui::MenuItem("Hemisphere"))
			{
				App->scene->CreateShape(HEMISPHERE, 9, 9);
			}
			if (ImGui::MenuItem("Icosahedron"))
			{
				App->scene->CreateShape(ICOSAHEDRON, 9, 9);
			}
			if (ImGui::MenuItem("Dodecahedron"))
			{
				App->scene->CreateShape(DODECAHEDRON, 9, 9);
			}
			if (ImGui::MenuItem("Octahedron"))
			{
				App->scene->CreateShape(OCTAHEDRON, 9, 9);
			}
			if (ImGui::MenuItem("Tetrahedron"))
			{
				App->scene->CreateShape(TETRAHEDRON, 9, 9);
			}
			if (ImGui::MenuItem("Rock"))
			{
				App->scene->CreateShape(ROCK, 9, 9);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
		
	}
}

// Add a new dummy node, child of the passed node or root node if parent is missing
// Returns pointer to the added node
HierarchyNode* Hierarchy::AddNode(GameObject* obj, HierarchyNode* parent)
{
	HierarchyNode* n = new HierarchyNode(obj, parent);

	parent->childs.push_back(n);
	parent->flags &= ~HierarchyNode::leaf_flags;
	parent->flags |= HierarchyNode::base_flags;

	n->LogAction("Added child");
	parent->LogAction("to parent");

	// General list of all nodes, not used for printing
	nodes.push_back(n);
	return n;

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
				if (ImGui::MenuItem("Rename"))
					node->is_rename = true;

				else if (ImGui::MenuItem("Delete"))
					DeleteNode(node);

				ImGui::EndPopup();
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
						for (HierarchyNode* i : Hierarchy::selected_nodes) // Selected nodes has selected state, need to unselect, toggle is safe [panaderia de pan]
							i->ToggleSelection();

						Hierarchy::selected_nodes.clear();
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

				if (node->childs.size() > 0) // Node is open, need to draw childs if has childs
					DrawNodes(node->childs);

				ImGui::TreePop();
			}
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

void Hierarchy::SetSceneName(const char* name)
{
	LOG("Renaming scene from '%s' to '%s'", scene_name, name);
	scene_name = name;
}
