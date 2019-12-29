#include "Text.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"

Text::Text(GameObject* gameObject, UI_Element::Type type) : UI_Element(UI_Element::Type::TEXT, gameObject)
{
	visible = true;
	interactable = false;
	draggable = false;

	if (!gameObject->HasComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS))
		canvas = (Canvas*)gameObject->AddComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);
	else
		canvas = (Canvas*)gameObject->GetComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);

	material = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);

	LoadFont("Assets/Fonts/Dukas.ttf", font_size);
	font.path = "Assets/Fonts/Dukas.ttf";
	canvas->AddElement(this);

	position2D.y = 150;
}


Text::~Text()
{
	font.clean();
}

void Text::LoadFont(const char* path, int size) 
{
	font.init(path, size /* size */);
}

void Text::Draw(ComponentCamera* camera)
{
	glColorColorF(color);

	glfreetype::print(camera, font, position2D.x, position2D.y, text);

}
void Text::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Text"))
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Visible", &visible);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Interactable", &interactable);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Draggable", &draggable);
		ImGui::Separator();

		ImGui::ColorEdit3("Color", (float*)&color);

		if (ImGui::DragFloat("Font size", &font_size, 1.0f, 0.0f, 100.0f, "%.2f")) {

			font.clean();
			font.init(font.path, font_size);
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Use with caution, may temporary freeze the editor with large numbers. \n It is recommended to directly input the number with the keyboard");


		ImGui::Text(text);
		
		if (ImGui::InputTextWithHint("TextChange",text, buffer, MAX_TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			strcpy_s(text, buffer);

		}

		// Size
		ImGui::Text("Size:    ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##textsize", &size2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##textsize", &size2D.y);

		// Position
		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##textposition", &position2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##textposition", &position2D.y);

		// Rotation
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("##textrotation", &rotation2D);

		// Image
		ImGui::Separator();
		ImGui::Text("Font: ");
		ImGui::Text(font.path);
		ImGui::SameLine();

		if (ImGui::Button("Load..."))
			ImGui::OpenPopup("Load Font");

		if (ImGui::BeginPopup("Load Font"))
		{
			if (ImGui::Selectable("Dukas")) {
				font.clean();
				LoadFont("Assets/Fonts/Dukas.ttf", DEFAULT_FONT_SIZE);
				font.path = "Assets/Fonts/Dukas.ttf";
			}
			if (ImGui::Selectable("Wintersoul")) {
				font.clean();
				LoadFont("Assets/Fonts/Wintersoul.ttf", DEFAULT_FONT_SIZE);
				font.path = "Assets/Fonts/Wintersoul.ttf";
			}
			if (ImGui::Selectable("EvilEmpire")) {
				font.clean();
				LoadFont("Assets/Fonts/EvilEmpire.otf", DEFAULT_FONT_SIZE);
				font.path = "Assets/Fonts/EvilEmpire.otf";
			}
			if (ImGui::Selectable("Smack")) {
				font.clean();
				LoadFont("Assets/Fonts/Smack.otf", DEFAULT_FONT_SIZE);
				font.path = "Assets/Fonts/Smack.otf";
			}
			ImGui::EndPopup();
		}

		if (!font.textures.empty())
			ImGui::Image((ImTextureID)font.textures.back(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		else
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "*Font not loaded*");

		ImGui::Separator();
		ImGui::Separator();
	}
}
