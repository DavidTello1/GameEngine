#include "Image.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

Image::Image(GameObject* gameObject, UI_Element::Type type) : UI_Element(UI_Element::Type::IMAGE, gameObject)
{
}

Image::~Image()
{
}

void Image::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Image"))
	{
		if (ImGui::Checkbox("Visible", &visible))
			SwitchVisible();

		if (ImGui::Checkbox("Interactable", &interactable))
			SwitchInteractable();

		if (ImGui::Checkbox("Draggable", &draggable))
			SwitchDraggable();

		ImGui::Separator();


	}
}