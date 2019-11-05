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

		ImGui::EndMenuBar();
	}

	// Child Hierarchy
	ImGui::BeginChild("Hierarchy", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 0), true);
	
	if (ImGui::TreeNode("Show All"))
	{
		ImGui::Selectable("Meshes");
		ImGui::Selectable("Materials");
		ImGui::Selectable("Models");

		ImGui::TreePop();
	}

	ImGui::Separator();

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

	ImGui::EndChild();
}

void Assets::ImportAsset()
{
}