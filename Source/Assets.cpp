#include "Application.h"
#include "ModuleFileSystem.h"
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

	UpdateAssets();
}

Assets::~Assets()
{
}

void Assets::Draw()
{
	// Child Hierarchy
	ImGui::BeginChild("Hierarchy", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 0), true, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Show All")) //Resource types
		{
			ImGui::MenuItem("Models");
			ImGui::MenuItem("Meshes");
			ImGui::MenuItem("Materials");
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Refresh"))
		{
			UpdateAssets(); // Update Assets Nodes
		}

		ImGui::EndMenuBar();
	}

	DrawHierarchy(assets); //Draw Hierarchy Tree
	ImGui::EndChild();
	ImGui::SameLine();

	// Child Icons
	ImGui::BeginChild("Icons", ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);

	//if (ImGui::BeginPopupContextItem("Right Click Options"))
	//{
	//	if (ImGui::BeginMenu("Create", selected_assets.empty()))
	//	{
	//		ImGui::MenuItem("Folder");
	//		ImGui::EndMenu();
	//	}

	//	if (ImGui::MenuItem("Remove", nullptr, false, !selected_assets.empty()))
	//	{
	//	}

	//	if (ImGui::MenuItem("Rename", nullptr, false, !selected_assets.empty()))
	//	{

	//	}
	//	ImGui::EndPopup();
	//}

	if (ImGui::BeginMenuBar()) //Show path
	{
		ImGui::Text("%s", current_node.path.c_str());
		ImGui::EndMenuBar();
	}
	//DrawIcons(); // Draw icons

	ImGui::EndChild();
}

void Assets::ImportAsset()
{
}

void Assets::UpdateAssets()
{
	//if (timer.ReadSec() > rate)
	//{
		std::vector<std::string> ignore_ext;
		ignore_ext.push_back("meta");
		assets = App->file_system->GetAllFiles("Assets", nullptr, &ignore_ext);
		timer.Start();
	//}
}

void Assets::DrawHierarchy(const PathNode& node)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (node.IsLastFolder() == true)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (current_node == node)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	if (node.file == false) //if folder is not empty
	{
		bool open = ImGui::TreeNodeEx(node.localPath.c_str(), nodeFlags, node.localPath.c_str());

		if (ImGui::IsItemClicked()) //current_node update
			current_node = node;

		if (open && !node.IsLastFolder())
		{
			for (uint i = 0; i < node.children.size(); i++)
			{
				DrawHierarchy(node.children[i]);
			}
			ImGui::TreePop();
		}
	}
}

void Assets::DrawIcons(const PathNode& node)
{

}