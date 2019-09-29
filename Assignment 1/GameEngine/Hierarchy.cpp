#include "Hierarchy.h"
#include "Console.h"
#include "Globals.h"

//Hierarchy hierarchy = Hierarchy();

char Hierarchy::scene_name[MAX_NAME_LENGTH];

std::vector<HierarchyNode> Hierarchy::nodes;
std::vector<HierarchyNode*> Hierarchy::selected_nodes;
Hierarchy::Hierarchy()
{
}

void Hierarchy::Init() 
{
	SetSceneName("Recursively");


	for (int i = 0; i < 6; i++) {
		HierarchyNode node = HierarchyNode();
		for (int j = 0; j < i; j++) {
			node.childs.push_back(HierarchyNode());
		}
		Hierarchy::nodes.push_back(node);
	}
	HierarchyNode node = HierarchyNode();
	for (int j = 0; j < 3; j++) {
		node.childs.push_back(HierarchyNode());
	}
	for (int j = 0; j < 4; j++) {
		node.childs[0].childs.push_back(HierarchyNode());
	}

	Hierarchy::nodes.push_back(node);
}

Hierarchy::~Hierarchy()
{
}

// Add a new dummy node, child of the passed node or an independent if is nullptr
void Hierarchy::AddNode(HierarchyNode* node) 
{
	HierarchyNode n = HierarchyNode(HierarchyNode::leaf_flags);

	if (node == nullptr) 
	{
		Hierarchy::nodes.push_back(n);
	}
	else {
		node->childs.push_back(n);
		node->flags &= ~HierarchyNode::leaf_flags;
		node->flags |= HierarchyNode::base_flags;
	}


}


void Hierarchy::ShowHierarchy(bool* open) 
{

	Draw("Hierarchy", open);
}


void DrawNodes(std::vector<HierarchyNode>& v)
{
	char buffer[512];
	for (HierarchyNode& node : v) 
	{

		//In future to be substituited buffer by node.name only, no need to show id
		sprintf_s(buffer, 512, "%s %ld", node.name, node.id);
		if (ImGui::TreeNodeEx(buffer, node.flags))
		{
			// if has childs
			if (node.childs.size() > 0) 
			{
				DrawNodes(node.childs);
			}
			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked()) 
		{
			if(node.selected_index == -1)
			{
				node.flags |= ImGuiTreeNodeFlags_Selected;
				LOG("NODE %ld SELECTED", node.id, 'd');
				Hierarchy::selected_nodes.push_back(&node);
				node.selected_index = Hierarchy::selected_nodes.size()-1;

			}
			// Does not work
			else {
				node.flags &= ~ImGuiTreeNodeFlags_Selected;
				Hierarchy::selected_nodes.erase(Hierarchy::selected_nodes.begin()+node.selected_index);
				for (HierarchyNode* n : Hierarchy::selected_nodes)
				{
					n->selected_index--;
				}
				node.selected_index = -1;
				LOG("NODE %ld UN-SELECTED", node.id, 'd');
			}
		}
		else
		{
			//LOG("NODE %ld NOT CLICKED", node.id, 'd');
		}


	}

}

void Hierarchy::Draw(const char * title, bool * p_open)
{

	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title, p_open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Add Node")) {
		AddNode();
	}ImGui::SameLine();
	if (ImGui::Button("Add Child")) {
		AddNode(Hierarchy::selected_nodes[0]);
	}ImGui::SameLine();
	if (ImGui::Button("Add Childs")) {
		for (HierarchyNode* selected : Hierarchy::selected_nodes)
			AddNode(selected);
	}

	ImGui::Text(Hierarchy::scene_name);
	ImGui::Separator();

	// Actually drawing of nodes

	//Hierarchy::selected_nodes.clear();
	DrawNodes(Hierarchy::nodes);

	// To delete
	if (ImGui::TreeNode("Hardcoded Nodes"))
	{
		ImGui::Columns(2, "tree", true);
		for (int x = 0; x < 3; x++)
		{
			bool open1 = ImGui::TreeNode((void*)(intptr_t)x, "Node%d", x);

			ImGui::NextColumn();
			if (ImGui::Button("V"))  // visible
			{
				LOG("Object set to invisible, or visible idk",'v');
			}
			ImGui::NextColumn();

			if (open1)
			{
				for (int y = 0; y < 3; y++)
				{
					bool open2 = ImGui::TreeNode((void*)(intptr_t)y, "Node%d.%d", x, y);
					ImGui::NextColumn();
					ImGui::Button("V");
					if (open2)
					{
						ImGui::Text("Even more contents");
						if (ImGui::TreeNode("Tree in column"))
						{
							ImGui::Text("The quick brown fox jumps over the lazy dog");
							ImGui::TreePop();
						}
					}
					ImGui::NextColumn();
					if (open2)
						ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		ImGui::Columns(1);
		ImGui::TreePop();
	}
	ImGui::End();
}

bool Hierarchy::SetSceneName(const char * name)
{
	if (strlen(name) < MAX_NAME_LENGTH) {
		sprintf_s(Hierarchy::scene_name, 512, "%s",name);
		return true;
	}
	else {
		Console::console.AddLog("[Error]: Scene name exceeds max length (512)");
		return false;

	}
}
