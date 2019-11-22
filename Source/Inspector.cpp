#include "Inspector.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentRenderer.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

Inspector::Inspector() : Panel("Inspector")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	has_menubar = true;
}

Inspector::~Inspector()
{
}

void Inspector::Draw()
{
	static bool rename = false;

	obj = App->scene->GetSelectedGameObject();
	if (obj == nullptr)
	{
		position = rotation = scale = float3::zero;
		return;
	}
	position = obj->GetPosition();
	rotation = obj->GetRotation();
	scale = obj->GetScale();


	if (ImGui::BeginMenuBar())
	{
		in_menu = false;

		if (ImGui::BeginMenu("Add"))
		{
			in_menu = true;

			if (ImGui::MenuItem("Mesh", nullptr, false, !obj->GetComponent(Component::Type::Mesh)))
				obj->AddComponent(Component::Type::Mesh);

			if (ImGui::MenuItem("Material", nullptr, false, !obj->GetComponent(Component::Type::Material)))
				obj->AddComponent(Component::Type::Material);

			if (ImGui::MenuItem("Renderer", nullptr, false, !obj->GetComponent(Component::Type::Renderer)))
				obj->AddComponent(Component::Type::Renderer);

			if (ImGui::MenuItem("Camera", nullptr, false, !obj->GetComponent(Component::Type::Camera)))
			{
				obj->AddComponent(Component::Type::Camera);
			}

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Remove", !obj->components.empty()))
		{
			in_menu = true;

			if (obj->GetComponent(Component::Type::Mesh))
			{
				if (ImGui::MenuItem("Mesh"))
					obj->DeleteComponent(Component::Type::Mesh);
			}
			if (obj->GetComponent(Component::Type::Material))
			{
				if (ImGui::MenuItem("Material"))
					obj->DeleteComponent(Component::Type::Material);
			}
			if (obj->GetComponent(Component::Type::Renderer))
			{
				if (ImGui::MenuItem("Renderer"))
					obj->DeleteComponent(Component::Type::Renderer);
			}
			if (obj->GetComponent(Component::Type::Camera))
			{
				if (ImGui::MenuItem("Camera"))
					obj->DeleteComponent(Component::Type::Camera);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (rename)
	{
		char buffer[NAME_LENGTH];
		sprintf_s(buffer, NAME_LENGTH, "%s", obj->GetName());

		if (ImGui::InputText("##GameObject", buffer, NAME_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			obj->SetName(buffer);
			rename = false;
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), obj->GetName());
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
		if (ImGui::Button("##rename", ImVec2(15, 15)))
			rename = true;

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Rename");
	}

	ImGui::Text("ID: %ld", obj->GetUID());
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Position
		ImGui::Text("Position");
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("x##1", &position.x, 0.5f))
			SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("y##1", &position.y, 0.5f))
			SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("z##1", &position.z, 0.5f))
			SetPosition(obj, position);

		ImGui::Separator();

		// Rotation
		ImGui::Text("Rotation");
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("x##2", &rotation.x, 0.5f, 0.0f, 360.0f))
			SetRotation(obj, rotation);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("y##2", &rotation.y, 0.5f, 0.0f, 360.0f))
			SetRotation(obj, rotation);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("z##2", &rotation.z, 0.5f, 0.0f, 360.0f))
			SetRotation(obj, rotation);

		ImGui::Separator();

		// Scale
		ImGui::Text("Scale");
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("x##3", &scale.x, 0.2f))
			SetScale(obj, scale);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("y##3", &scale.y, 0.2f))
			SetScale(obj, scale);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("z##3", &scale.z, 0.2f))
			SetScale(obj, scale);

		ImGui::Separator();
		ImGui::NewLine();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("BoundingBox", &obj->show_bounding_box);

		ImGui::Separator();
	}

	// Draw Components
	for (uint i = 0; i < obj->components.size(); ++i)
	{
		obj->components[i]->DrawInspector();
	}
}
