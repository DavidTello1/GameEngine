#include "Canvas.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"


Canvas::Canvas(GameObject* gameObject, UI_Element::Type type) : UI_Element(UI_Element::Type::CANVAS, gameObject)
{
	visible = true;
	interactable = false;
	draggable = false;

	material = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
}


Canvas::~Canvas()
{
}

void Canvas::AddElement(UI_Element* element)
{
	elements.push_back(element);
}

void Canvas::Draw() 
{
	for (int i = 0; i < elements.size(); i++)
	{
		elements[i]->Draw();
	}
}

void Canvas::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Canvas"))
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Visible", &visible);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Interactable", &interactable);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Draggable", &draggable);
		ImGui::Separator();

		// Size
		ImGui::Text("Size:    ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##size", &size2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##size", &size2D.y);

		// Position
		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##position2D", &position2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##position2D", &position2D.y);

		// Rotation
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("##rotation2D", &rotation2D);

		// Scale
		ImGui::Text("Scale:   ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##scale2D", &scale2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##scale2D", &scale2D.y);

		ImGui::Separator();
	}
}