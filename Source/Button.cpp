#include "Button.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

Button::Button(GameObject* gameObject, UI_Element::Type type) : UI_Element(UI_Element::Type::BUTTON, gameObject)
{
	size2D = { 40,20 };

	visible = true;
	interactable = true;
	draggable = false;

	if (!gameObject->HasComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS))
		canvas = (Canvas*)gameObject->AddComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);
	else
		canvas = (Canvas*)gameObject->GetComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);

	material = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	font.init("Assets/Fonts/Dukas.ttf", font_size);
	font.path = "Assets/Fonts/Dukas.ttf";

	canvas->AddElement(this);
}

Button::~Button()
{
}

void Button::Draw(ComponentCamera* camera)
{
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(position2D.x, position2D.y, 1);
	glMultTransposeMatrixf(camera->origin_view_matrix);

	glColorColorF(color);

	glBindTexture(GL_TEXTURE_2D, material->tex_id);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	// Draw A Quad
	glTexCoord2i(0, 0); glVertex2f(-size2D.x, -size2D.y);    // Top Left		glVertex2i(100, 100);
	glTexCoord2i(0, 1); glVertex2f(-size2D.x, size2D.y);    // Top Right		glVertex2i(100, 500);
	glTexCoord2i(1, 1); glVertex2f(size2D.x, size2D.y);    // Bottom Right	glVertex2i(500, 500);
	glTexCoord2i(1, 0); glVertex2f(size2D.x, -size2D.y);    // Bottom Left	glVertex2i(500, 100);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColorColorF(Color::white);

	glEnd();
	glPopMatrix();

	glColorColorF(text_color);
	glfreetype::print(camera, font, position2D.x + text_pos.x, position2D.y + text_pos.y, text);
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
		ImGui::DragFloat("x##buttonsize", &size2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##buttonsize", &size2D.y);

		// Position
		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##buttonposition", &position2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##buttonposition", &position2D.y);

		// Rotation
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("##buttonrotation", &rotation2D);

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
		ImGui::ColorEdit4("##Idle", (float*)&idle_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Idle");
		if (state == IDLE) ChangeColor(idle_color);

		ImGui::ColorEdit4("##Hovered", (float*)&hovered_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Hovered");
		if (state == HOVERED) ChangeColor(hovered_color);

		ImGui::ColorEdit4("##Selected", (float*)&selected_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Selected");
		if (state == SELECTED || state == DRAGGING) ChangeColor(selected_color);

		ImGui::ColorEdit4("##Locked", (float*)&locked_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Locked");
		if (state == LOCKED) ChangeColor(locked_color);

		// Text
		ImGui::Separator();
		if (ImGui::DragFloat("Font size", &font_size, 1.0f, 0.0f, 100.0f, "%.2f")) {

			font.clean();
			font.init(font.path, font_size);
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Use with caution, may temporary freeze the editor with large numbers. \n It is recommended to directly input the number with the keyboard");

		ImGui::Text("Text");
		ImGui::InputText("##buttontext", text, MAX_TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::ColorEdit3("Color", (float*)&text_color);

		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##buttontextposition", &text_pos.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##buttontextposition", &text_pos.y);


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