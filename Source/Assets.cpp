#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ResourceMaterial.h"
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
	// Child Hierarchy -------------------------------
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

	// Child Icons -------------------------------
	ImGui::BeginChild("Icons", ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);

	//if (ImGui::BeginPopupContextItem("Icons"))
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
		ImGui::Text(current_node.path.c_str());
		ImGui::EndMenuBar();
	}
	DrawIcons(current_node); // Draw Icons
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
	for (uint i = 0; i < node.children.size(); i++)
	{
		// Change border color if selected
		if (selected_node == node.children[i])
			border_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
		else
			border_color = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);

		// Draw
		const int size = 75;
		const int padding = 15;
		int columns = (int)ImGui::GetWindowWidth() / size;
		int total_size = (columns*size) + (padding * (columns - 1));

		if (total_size > ImGui::GetWindowWidth())
			columns--;

		if (i % columns != 0)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);

		ImGui::SetNextItemWidth(size);
		ImGui::BeginGroup();
		ImGui::Image((ImTextureID)GetIcon(node.children[i]), ImVec2(ICON_WIDTH, ICON_HEIGHT), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), border_color);

		// Text size
		std::string text = node.children[i].localPath;
		std::string dots = "...";

		if (ImGui::CalcTextSize(text.c_str()).x > size)
		{
			text = text.substr(0, 7);
			text.append(dots);
		}
		ImGui::Text(text.c_str());
		ImGui::EndGroup();

		if ((i + 1) % columns != 0)
			ImGui::SameLine();

		// Item clicking
		if (ImGui::IsItemClicked())
			selected_node = node.children[i];

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) //open
		{
			if (node.file == false) //if folder
			{
				next_node = node.children[i];
				current_node = next_node;
			}
			//else
			//{
			//	std::string metaFile = current_node.path + (".meta");
			//	uint64 id = App->resources->GetIDFromMeta(metaFile.c_str());
			//	Resource* resource = App->resources->GetResource(id);

			//	if (resource && resource->GetType() == Resource::PREFAB)
			//	{
			//		App->moduleResources->LoadPrefab(node.path.c_str());
			//	}
			//}
		}
	}
}

uint Assets::GetIcon(const PathNode& node)
{
	if (node.file == false) //if folder
		return folder_icon;

	//else
	//{
	//	Resource* resource = App->resources->GetResource(node.id); //get resource

	//	if (resource->GetType() == Resource::Type::model) //if model
	//		return model_icon;

	//	else if (resource->GetType() == Resource::Type::material) //if material
	//		return material_icon;

	//	else if (resource->GetType() == Resource::Type::scene) //if scene
	//		return scene_icon;
	//}
}