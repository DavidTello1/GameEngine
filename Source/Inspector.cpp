#include "Inspector.h"
#include "Imgui/imgui.h"


Inspector::Inspector() : Panel("Inspector")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;
}


Inspector::~Inspector()
{
}

void Inspector::Draw() {

	float dummy = 0;
	float min = -340000000;
	float max = 340000000;
	ImGui::Text("Transform");
	ImGui::Separator();

	ImGui::Columns(1, "##value", false);
	// Translate
	ImGui::Text("Translation");
	ImGui::Columns(3, "##value", false);
	ImGui::InputFloat("X",&dummy,min,max);
	ImGui::NextColumn();
	ImGui::InputFloat("Y", &dummy, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Z", &dummy, min, max);
	ImGui::NextColumn();

	ImGui::Columns(1,"##value",false);
	// Rotate
	ImGui::Text("Rotation");
	ImGui::Columns(3, "##value", false);
	ImGui::InputFloat("X", &dummy, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Y", &dummy, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Z", &dummy, min, max);
	ImGui::NextColumn();

	ImGui::Columns(1, "##value", false);
	// Scale
	ImGui::Text("Scaling");
	ImGui::Columns(3, "##value", false);
	ImGui::InputFloat("X", &dummy, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Y", &dummy, min, max);
	ImGui::NextColumn();
	ImGui::InputFloat("Z", &dummy, min, max);
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::Text("Advanced");
	ImGui::Text("Triangle count: %d", 9564);
	ImGui::Text("Triangle count: %d", 9564);
	ImGui::Text("Triangle count: %d", 9564);
	ImGui::Text("Triangle count: %d", 9564);

}