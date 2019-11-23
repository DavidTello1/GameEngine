#include "Inspector.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "ModuleScene.h"
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
	rotation = obj->GetRotation() * RADTODEG;
	scale = obj->GetScale();


	if (ImGui::BeginMenuBar())
	{

		if (ImGui::BeginMenu("Add"))
		{
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
		if (ImGui::BeginPopupContextItem("Transform"))
		{
			if (ImGui::MenuItem("Reset"))
				obj->ResetTransform();
			ImGui::EndPopup();
		}
		ImGui::Checkbox("Static", &obj->is_static);

		if (obj->is_static)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		
		// Position
		ImGui::Text("Position");
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("x##1", &position.x, precision, -inf, inf, precision_char))
		if (!obj->is_static) SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("y##1", &position.y, precision,-inf,inf, precision_char))
		if (!obj->is_static) SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		
		if (ImGui::DragFloat("z##1", &position.z, precision, -inf, inf, precision_char))
		if (!obj->is_static) SetPosition(obj, position);

		ImGui::Separator();

		// Rotation
		ImGui::Text("Rotation");
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("x##2", &rotation.x, precision, -inf, inf, precision_char))
		if (!obj->is_static) SetRotation(obj, rotation*DEGTORAD);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
	
		if (ImGui::DragFloat("y##2", &rotation.y, precision, -inf, inf, precision_char))
		if (!obj->is_static) SetRotation(obj, rotation*DEGTORAD);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("z##2", &rotation.z, precision, -inf, inf, precision_char))
		if (!obj->is_static) SetRotation(obj, rotation*DEGTORAD);

		ImGui::Separator();

		// Scale
		ImGui::Text("Scale");
		ImGui::SetNextItemWidth(60);
		
		if (ImGui::DragFloat("x##3", &scale.x, precision, -inf, inf, precision_char))
		{
			if (!obj->is_static) {
				if (lock_scale)
				{
					float diff = scale.x - obj->GetScale().x;
					SetScale(obj, { scale.x, scale.y + diff, scale.z + diff });
				}
				else
					SetScale(obj, scale);
			}
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("y##3", &scale.y, precision, -inf, inf, precision_char))
		{
			if (!obj->is_static) {
				if (lock_scale)
				{
					float diff = scale.y - obj->GetScale().y;
					SetScale(obj, { scale.x + diff, scale.y, scale.z + diff });
				}
				else
					SetScale(obj, scale);
			}
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		
		if (ImGui::DragFloat("z##3", &scale.z, precision, -inf, inf, precision_char))
		{
			if (!obj->is_static) {
				if (lock_scale)
				{
					float diff = scale.z - obj->GetScale().z;
					SetScale(obj, { scale.x + diff, scale.y + diff,scale.z });
				}
				else
					SetScale(obj, scale);
			}
		}

		ImGui::SameLine();
		//ImGui::SetNextItemWidth(60);
		ImGui::Checkbox("lock##1", &lock_scale);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Lock scale");

		ImGui::Separator();

		if (obj->is_static)
		{
			ImGui::PopStyleVar();
		}
	}

	// Draw Components
	for (uint i = 0; i < obj->components.size(); ++i)
	{
		obj->components[i]->DrawInspector();
	}
}
