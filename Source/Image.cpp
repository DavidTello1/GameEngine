#include "Image.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"

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

	material = (ResourceMaterial*)App->resources->CreateResource(Resource::Type::material);
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

		// Image
		ImGui::Separator();
		ImGui::Text("Image");
		ImGui::SameLine();

		if (ImGui::Button("Load..."))
			ImGui::OpenPopup("Load Image");

		if (ImGui::BeginPopup("Load Image"))
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

		ImGui::Separator();
		ImGui::Separator();
	}
}