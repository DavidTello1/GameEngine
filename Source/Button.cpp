#include "Button.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

Button::Button(GameObject* gameObject, UI_Element::Type type) : UI_Element(UI_Element::Type::BUTTON, gameObject)
{
	visible = true;
	interactable = true;
	draggable = false;

	if (!gameObject->HasComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS))
		canvas = (Canvas*)gameObject->AddComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);
	else
		canvas = (Canvas*)gameObject->GetComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);

}

Button::~Button()
{
}

void Button::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Button"))
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

		// Button
		ImGui::Separator();
		ImGui::Text("Button");
		ImGui::SameLine();

		if (ImGui::Button("Load Image..."))
			ImGui::OpenPopup("Load Button");

		if (ImGui::BeginPopup("Load Button"))
		{
			std::vector<Resource*> images = App->resources->GetAllResourcesOfType(Resource::Type::material);
			for (int i = 0; i < images.size(); i++)
			{
				if (strcmp("", images[i]->GetName().c_str()) == 0 || 
					images[i]->GetName().substr(images[i]->GetName().size() - 3) == "fbx" ||
					images[i]->GetName().substr(images[i]->GetName().size() - 3) == "FBX")
					continue;

				if (ImGui::Selectable(images[i]->GetName().c_str()))
					material->LoadTexture(images[i]->GetFile());
			}
			ImGui::EndPopup();
		}

		ImGui::Text("Hovered");
		ImGui::Text("Selected");
		ImGui::Text("Dragging");
		ImGui::Text("Locked");

		ImGui::Separator();
		ImGui::Separator();
	}
}