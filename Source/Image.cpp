#include "Image.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

Image::Image(GameObject* gameObject, UI_Element::Type type) : UI_Element(UI_Element::Type::IMAGE, gameObject)
{
	visible = true;
	interactable = true;
	draggable = false;

	if (!gameObject->HasComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS))
		canvas = (Canvas*)gameObject->AddComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);
	else
		canvas = (Canvas*)gameObject->GetComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);
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
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Visible", &visible);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Interactable", &interactable);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Draggable", &draggable);
		ImGui::Separator();

		// Position
		if (ImGui::TreeNode("Position"))
		{
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("x##position2D", &position2D.x))
			{
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("y##position2D", &position2D.y))
			{
			}

			// Anchor
			ImGui::Text("Anchor: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("x##anchor", &anchor.x))
			{
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("y##anchor", &anchor.y))
			{
			}

			ImGui::Separator();
			ImGui::TreePop();
		}

		// Rotation
		if (ImGui::TreeNode("Rotation"))
		{
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("##rotation2D", &rotation2D))
			{
			}

			// Pivot
			ImGui::Text("Pivot: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("x##pivot", &pivot.x))
			{
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("y##pivot", &pivot.y))
			{
			}

			ImGui::Separator();
			ImGui::TreePop();
		}

		// Scale
		if (ImGui::TreeNode("Scale"))
		{
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("x##scale2D", &scale2D.x))
			{
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("y##scale2D", &scale2D.y))
			{
			}

			// Size
			ImGui::Text("Size: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("x##size", &size2D.x))
			{
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			if (ImGui::DragFloat("y##size", &size2D.y))
			{
			}

			ImGui::Separator();
			ImGui::TreePop();
		}

		// Image
		ImGui::Separator();
		ImGui::Text("Image");
		if (material != nullptr)
			ImGui::Image((ImTextureID)material->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

	}
}