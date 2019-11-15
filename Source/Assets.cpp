#include "Assets.h"
#include "mmgr/mmgr.h"

using namespace std;

// ---------------------------------------------------------
Assets::Assets() : Panel("Assets")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	has_menubar = true;
}

Assets::~Assets()
{
}

void Assets::Draw()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
		{
			ImGui::MenuItem("Folder");
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Remove", nullptr, false, !selected_assets.empty()))
		{
		}

		if (ImGui::MenuItem("Rename", nullptr, false, !selected_assets.empty()))
		{

		}

		if (ImGui::MenuItem("Update Changes"))
		{
			//update changes from /Assets folder
		}
		ImGui::EndMenuBar();
	}

	// Child Hierarchy
	ImGui::BeginChild("Hierarchy", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 0), true, ImGuiWindowFlags_MenuBar);
	
	if (ImGui::BeginMenu("Show All")) //Resource types
	{
		ImGui::MenuItem("Models");
		ImGui::MenuItem("Meshes");
		ImGui::MenuItem("Materials");
		ImGui::EndMenu();
	}

	if (ImGui::TreeNode("Assets"))
	{
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::SameLine();


	// Child Icons
	ImGui::BeginChild("Icons", ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);
	
	if (ImGui::BeginMenuBar())
	{
		//ImGui::Text("%s>%s", folder_name.parent, folder_name.parent.parent, ...);

		ImGui::EndMenuBar();
	}
	// actual icons

	ImGui::EndChild();
}

void Assets::ImportAsset()
{
}