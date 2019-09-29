#include "Hierarchy.h"
#include "Console.h"
#include "ImGui/imgui.h"
#include "Globals.h"


//Hierarchy hierarchy = Hierarchy();

char Hierarchy::scene_name[MAX_NAME_LENGTH];

std::vector<HierarchyNode> Hierarchy::nodes;

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



void Hierarchy::ShowHierarchy(bool* open) 
{

	Draw("Hierarchy", open);
}

//void DrawNodes(const std::vector<HierarchyNode>& v) 
//{
//	char buffer[512];
//
//	for (int i = 0; i <v.size(); i++) {
//
//		sprintf_s(buffer, 512, "Node %ld", v[i].id);
//		if (ImGui::TreeNode(buffer))
//		{
//			for (int j = 0; j < v[i].childs.size(); j++)
//			{
//				sprintf_s(buffer, 512, "Child %ld", v[i].childs[j].id);
//				if (ImGui::TreeNode(buffer)) {
//
//					ImGui::TreePop();
//				}
//			}
//			ImGui::TreePop();
//		}
//	}
//}

void DrawNodes(const std::vector<HierarchyNode>& v)
{
	char buffer[512];

	for (const HierarchyNode node : v) 
	{
		// Get node name
		sprintf_s(buffer, 512, "%s %ld", node.name, node.id);

		if (ImGui::TreeNode(buffer))
		{
			if (node.childs.size() > 0)
			{
				DrawNodes(node.childs);
			}
			ImGui::TreePop();
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

	ImGui::Text(Hierarchy::scene_name);
	ImGui::Separator();

	// Actually drawing of nodes
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	
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
