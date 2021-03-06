#include "Application.h"
#include "ModuleScene.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ResourceMaterial.h"
#include "ResourceModel.h"
#include "ComponentMaterial.h"
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

	UpdateFilters(models);
	UpdateFilters(materials);
	UpdateFilters(scenes);

	current_node = assets;
}

Assets::~Assets()
{
}

void Assets::Draw()
{
	if (timer.ReadSec() > REFRESH_RATE) // Update Assets Hierarchy
	{
		UpdateAssets();
		timer.Start();
	}

	// Child Hierarchy -------------------------------
	ImGui::BeginChild("Hierarchy", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 0), true, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Show All")) //Resource types
		{
			if (ImGui::MenuItem("Models", NULL, &filter_models, models.children.size() > 0))
			{
				UpdateFilters(models);
				filter_models = true;
				filter_scenes = false;
				filter_materials = false;
			}
			if (ImGui::MenuItem("Materials", NULL, &filter_materials, materials.children.size() > 0))
			{
				UpdateFilters(materials);
				filter_models = false;
				filter_materials = true;
				filter_scenes = false;
			}
			if (ImGui::MenuItem("Scenes", NULL, filter_scenes, scenes.children.size() > 0))
			{
				UpdateFilters(scenes);
				filter_models = false;
				filter_materials = false;
				filter_scenes = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Import", NULL, false, (selected_node.path != "" && selected_node.file == true)))
		{
			ImportAsset(selected_node); // Load Asset
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
		if (filter_models)
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Models");
		
		else if (filter_materials)
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Materials");
		
		else if (filter_scenes)
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Scenes");
		
		else if (selected_node.path != "")
			ImGui::Text(selected_node.path.c_str());

		else
			ImGui::Text(current_node.path.c_str());

		ImGui::EndMenuBar();
	}
	DrawIcons(current_node); // Draw Icons
	ImGui::EndChild();
}

void Assets::ImportAsset(const PathNode& node)
{
	UID id = App->resources->GetIDFromMeta(std::string(node.path + (".meta")).c_str());
	Resource* resource = App->resources->GetResource(id);

	if (resource && resource->GetType() == Resource::material) // ***ERROR IMPORTING WHEN FILE EXISTS IN LIBRARY_FOLDER
	{
		GameObject* obj = App->scene->GetSelectedGameObject();
		if (obj != nullptr)
		{
			ComponentMaterial* mat = (ComponentMaterial*)obj->GetComponent(Component::Type::Material);
			if (mat == nullptr)
				mat = (ComponentMaterial*)obj->AddComponent(Component::Type::Material);

			resource->LoadToMemory();
			mat->SetMaterial((ResourceMaterial*)resource);
		}
	}
	else if (resource && resource->GetType() == Resource::model)
	{
		resource->LoadToMemory();
		ResourceModel* res = (ResourceModel*)resource;
		res->CreateGameObjects(App->file_system->GetFileName(node.path.c_str()).c_str()); //create gameobjects from model
		App->scene->RedoQuatree();
	}
}

void Assets::UpdateAssets()
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	assets = App->file_system->GetAllFiles("Assets", nullptr, &ignore_ext);
}

void Assets::UpdateFilters(PathNode& node)
{
	std::vector<std::string> filter_ext;

	if (node.children == models.children)
	{
		filter_ext.push_back("fbx");
		filter_ext.push_back("obj");
	}
	else if (node.children == materials.children)
	{
		filter_ext.push_back("png");
		filter_ext.push_back("dds");
		filter_ext.push_back("jpg");
		filter_ext.push_back("tga");
	}
	else if (node.children == scenes.children)
	{
		filter_ext.push_back("dvs");
	}

	node = App->file_system->GetAllFiles("Assets", &filter_ext);
	FilterFolders(node, node);

	uint size = node.children.size();
	uint cont = 0;
	for (uint i = 0; i < size; ++i)
	{
		uint j = i - cont;
		if (node.children[j].file == false)
		{
			node.children.erase(node.children.begin() + j);
			cont++;
		}
	}

	current_node = node;
}

void Assets::FilterFolders(PathNode& node, PathNode& parent)
{
	for (uint i = 0; i < node.children.size(); ++i)
	{
		if (node.children[i].file == false) // if folder filter again
			FilterFolders(node.children[i], node);

		else if (parent.children != node.children)//if file
			parent.children.push_back(node.children[i]); //add children to node
	}
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
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (node.path == current_node.path && !filter_models && !filter_materials && !filter_scenes)
		current_node = node;

	if (node.file == false) //if folder is not empty
	{
		bool open = ImGui::TreeNodeEx(node.file_name.c_str(), nodeFlags, node.file_name.c_str());

		if (ImGui::IsItemClicked()) //current_node update
		{
			current_node = node;
			filter_models = filter_materials = filter_scenes = false;
		}

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
		const int size = 84;
		const int spacing = 8;
		int columns = ((int)ImGui::GetWindowWidth() - spacing) / (size + spacing);

		ImGui::SetNextItemWidth(size);
		ImGui::BeginGroup();
		ImGui::Image((ImTextureID)GetIcon(node.children[i]), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), border_color);

		// Text size
		std::string text = node.children[i].file_name;
		std::string dots = "...";
		
		uint text_size = ImGui::CalcTextSize(text.c_str()).x;
		uint max_size = (size - ImGui::CalcTextSize(dots.c_str()).x) / 7;
		if (text_size > size)
		{
			text = text.substr(0, max_size);
			text.append(dots);
		}
		else if (text_size < size)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((size - text_size) / 2));

		ImGui::Text(text.c_str());
		ImGui::EndGroup();

		if ((i + 1) % columns != 0)
			ImGui::SameLine();

		// Item clicking
		if (ImGui::IsItemClicked())
		{
			selected_node = node.children[i];
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && selected_node.file == false) //open folder
		{
			next_node = node.children[i];
			current_node = next_node;
		}
	}
}

uint Assets::GetIcon(const PathNode& node)
{
	if (node.file == false) //if folder
		return folder_icon;

	else
	{
		std::string metaFile = node.path + (".meta");
		uint64 resourceID = App->resources->GetIDFromMeta(metaFile.c_str());
		Resource* resource = App->resources->GetResource(resourceID);

		if (resource != nullptr)
		{
			if (resource->GetType() == Resource::Type::model) //if model
				return model_icon;

			else if (resource->GetType() == Resource::Type::material) //if material
				return material_icon;

			else if (resource->GetType() == Resource::Type::scene) //if scene
				return scene_icon;
		}
		
		return file_icon;
	}
}

Resource* Assets::GetSelectedResource()
{
	if (selected_node.path != "" && selected_node.file == true)
	{
		std::string metaFile = selected_node.path + (".meta");
		uint64 resourceID = App->resources->GetIDFromMeta(metaFile.c_str());
		return App->resources->GetResource(resourceID);
	}
	return nullptr;
}