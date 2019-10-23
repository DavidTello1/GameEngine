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

	active = false; //remove when panel assets is not empty
}

Assets::~Assets()
{
}

void Assets::Draw()
{
	ImGui::BeginChild("Hierarchy", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, ImGui::GetWindowHeight()*0.8f), true);
	//...
	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginChild("Icons", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.78f, ImGui::GetWindowHeight()*0.8f), true);
	//...
	ImGui::EndChild();
}

void Assets::ImportAsset()
{
}