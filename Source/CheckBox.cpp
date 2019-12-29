#include "CheckBox.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

CheckBox::CheckBox(GameObject* gameObject, UI_Element::Type type) : UI_Element(UI_Element::Type::CHECKBOX, gameObject)
{
	size2D = { 20,20 };

	visible = true;
	interactable = true;
	draggable = false;

	if (!gameObject->HasComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS))
		canvas = (Canvas*)gameObject->AddComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);
	else
		canvas = (Canvas*)gameObject->GetComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);

	unselected = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	selected = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
	font.init("Assets/Fonts/Dukas.ttf", font_size);
	font.path = "Assets/Fonts/Dukas.ttf";

	canvas->AddElement(this);
}

CheckBox::~CheckBox()
{
}

void CheckBox::Draw(ComponentCamera* camera)
{
	if (state == SELECTED)
	{
		is_selected = !is_selected;
	}

	glPushMatrix();
	glLoadIdentity();

	glTranslatef(position2D.x, position2D.y, 1);
	glMultTransposeMatrixf(camera->origin_view_matrix);

	glColorColorF(color);

	if (is_selected)
		glBindTexture(GL_TEXTURE_2D, selected->tex_id);
	else
		glBindTexture(GL_TEXTURE_2D, unselected->tex_id);

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

void CheckBox::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("CheckBox"))
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
		ImGui::Text("Unselected");
		if (ImGui::Button("Load Image...##1"))
			ImGui::OpenPopup("Load CheckBox ##1");

		if (ImGui::BeginPopup("Load CheckBox ##1"))
		{
			std::vector<Resource*> images = App->resources->GetAllResourcesOfType(Resource::Type::material);
			for (int i = 0; i < images.size(); i++)
			{
				if (strcmp("", images[i]->GetName().c_str()) == 0 ||
					images[i]->GetName().substr(images[i]->GetName().size() - 3) == "fbx" ||
					images[i]->GetName().substr(images[i]->GetName().size() - 3) == "FBX")
					continue;

				if (ImGui::Selectable(images[i]->GetName().c_str()))
					unselected->LoadTexture(images[i]->GetFile());
			}
			ImGui::EndPopup();
		}

		if (unselected->tex_id != 0)
			ImGui::Image((ImTextureID)unselected->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		else
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "*Image not loaded*");

		ImGui::Text("Selected");
		if (ImGui::Button("Load Image... ##2"))
			ImGui::OpenPopup("Load CheckBox ##2");

		if (ImGui::BeginPopup("Load CheckBox ##2"))
		{
			std::vector<Resource*> images = App->resources->GetAllResourcesOfType(Resource::Type::material);
			for (int i = 0; i < images.size(); i++)
			{
				if (strcmp("", images[i]->GetName().c_str()) == 0 ||
					images[i]->GetName().substr(images[i]->GetName().size() - 3) == "fbx" ||
					images[i]->GetName().substr(images[i]->GetName().size() - 3) == "FBX")
					continue;

				if (ImGui::Selectable(images[i]->GetName().c_str()))
					selected->LoadTexture(images[i]->GetFile());
			}
			ImGui::EndPopup();
		}

		if (selected->tex_id != 0)
			ImGui::Image((ImTextureID)selected->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
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
		ImGui::InputText("##checkboxtext", text, MAX_TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::ColorEdit3("Color", (float*)&text_color);

		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##checkboxtextposition", &text_pos.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##checkboxtextposition", &text_pos.y);


		// Action
		ImGui::Separator();
		if (ImGui::Button("Action##checkbox"))
			ImGui::OpenPopup("Load Action##checkbox");

		if (ImGui::BeginPopup("Load Action##checkbox"))
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
