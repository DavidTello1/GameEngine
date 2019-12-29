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

	material = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
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

		// ------------------------------------------

		// Image
		ImGui::Separator();
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

		if (material->tex_id != 0)
			ImGui::Image((ImTextureID)material->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		else
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "*Image not loaded*");

		// States (Colors)
		ImGui::Separator();
		static ImVec4 idle_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::ColorEdit4("##Idle", (float*)&idle_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Idle");

		static ImVec4 hovered_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::ColorEdit4("##Hovered", (float*)&hovered_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Hovered");

		static ImVec4 selected_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::ColorEdit4("##Selected", (float*)&selected_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Selected");

		static ImVec4 locked_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::ColorEdit4("##Locked", (float*)&locked_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Locked");

		// Text
		ImGui::Separator();
		const int text_size = 120;
		static char text[text_size] = "Button";
		ImGui::Text("Text");
		ImGui::InputText("##text", text, text_size, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		// Action
		ImGui::Separator();
		if (ImGui::Button("Action"))
			ImGui::OpenPopup("Load Action");

		if (ImGui::BeginPopup("Load Action"))
		{
			for (int i = 0; i < sizeof(action_list) / sizeof(const char*); i++)
			{
				if (ImGui::Selectable(action_list[i]))
					action = (Action)i;
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), action_list[action]);

		ImGui::Separator();
		ImGui::Separator();
	}
}