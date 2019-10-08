#include "Viewport.h"
#include "Imgui/imgui.h"


Viewport::Viewport() : Panel("Viewport")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

}


Viewport::~Viewport()
{
}

void Viewport::Draw() 
{
	ImGui::GetWindowDrawList()->AddImage(
		(ImTextureID*)texture, 
		ImVec2(pos_x, pos_y), 
		ImVec2(pos_x + width, pos_y + height));
}
