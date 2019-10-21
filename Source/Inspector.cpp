#include "Inspector.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "GameObject.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

Inspector::Inspector() : Panel("Inspector")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	min = -3000;
	max = 3000;
}

Inspector::~Inspector()
{
}

void Inspector::Draw() 
{
	if (App->editor->GetSelectedGameobj() == nullptr)
		translation = rotation = scale = float3::zero;

	ImGui::Text("Transform");
	ImGui::Separator();

	// Translate
	ImGui::Columns(1, "##value", false);
	ImGui::Text("Translation");
	ImGui::Columns(3, "##value", false);
	ImGui::InputFloat("X", &translation.x, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Y", &translation.y, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Z", &translation.z, min, max);
	ImGui::NextColumn();

	// Rotate
	ImGui::Columns(1,"##value",false);
	ImGui::Text("Rotation");
	ImGui::Columns(3, "##value", false);
	ImGui::InputFloat("X", &rotation.x, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Y", &rotation.y, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Z", &rotation.z, min, max);
	ImGui::NextColumn();

	// Scale
	ImGui::Columns(1, "##value", false);
	ImGui::Text("Scale");
	ImGui::Columns(3, "##value", false);
	ImGui::InputFloat("X", &scale.x, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Y", &scale.y, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Z", &scale.z, min, max);
	ImGui::NextColumn();

	//ImGui::Separator();
	//ImGui::Text("Advanced");
	//ImGui::Text("Triangle count: %d", 9564);

}