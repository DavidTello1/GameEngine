#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentRenderer.h"

#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(GameObject* gameobj) : Component(Component::Type::Material, gameobj)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check2", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Material"))
	{
		if (GetMaterial() == nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
			ImGui::TextWrapped("Material not loaded");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::Text("Path: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", GetMaterial()->GetFile());

			ImGui::Text("Size: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", GetMaterial()->tex_width);
			ImGui::SameLine();
			ImGui::Text("x");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", GetMaterial()->tex_width);

			if (ImGui::TreeNode("Image"))
			{
				ImGui::Image((ImTextureID)GetMaterial()->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::TreePop();
			}

			ComponentRenderer* renderer = (ComponentRenderer*)object->GetComponent(Component::Type::Renderer);
			if (renderer != nullptr)
			{
				ImGui::NewLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
				ImGui::Checkbox("Checkers Material", &renderer->show_checkers);
			}
		}
		ImGui::Separator();
	}
}