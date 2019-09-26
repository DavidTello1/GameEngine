#include "Hierarchy.h"
#include "Console.h"
#include "ImGui/imgui.h"
#include "Globals.h"


Hierarchy hierarchy = Hierarchy();
char Hierarchy::scene_name[MAX_NAME_LENGTH];

Hierarchy::Hierarchy()
{
	SetSceneName("SceneName");
}


Hierarchy::~Hierarchy()
{
}



void Hierarchy::ShowHierarchy(bool* open) 
{

	hierarchy.Draw(Hierarchy::scene_name, open);
}

void Hierarchy::Draw(const char * title, bool * p_open)
{

	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title, p_open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::TreeNode("Miau"))
	{
		ImGui::Columns(2, "tree", true);
		for (int x = 0; x < 3; x++)
		{
			bool open1 = ImGui::TreeNode((void*)(intptr_t)x, "Node%d", x);
			ImGui::NextColumn();

			ImGui::Button("V"); // visible
			ImGui::SameLine();
			ImGui::Button("L"); // lock
			ImGui::SameLine();
			ImGui::Button("C"); // center camera to object

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
		sprintf_s(Hierarchy::scene_name, 512, "Hierarchy - %s",name);
		return true;
	}
	else {
		Console::console.AddLog("[Error]: Scene name exceeds max length (512)");
		return false;

	}
}
