#include "Inspector.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentRenderer.h"
#include "Assets.h"

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
	obj = App->scene->GetSelectedGameObject();
	res = App->editor->tab_assets->GetSelectedResource();

	if (obj != nullptr)
		DrawComponents(obj, res != nullptr);

	if (res != nullptr)
		DrawResource(res);
}

void Inspector::DrawResource(Resource* res)
{
	ImGui::BeginChild("Resource", ImVec2(0, 0), false, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) //Show path
	{
		ImGui::Text("Resources");
		ImGui::EndMenuBar();
	}

	//ImGui::Text("Name: ");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), res->GetName());

	ImGui::Text("ID: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%d", res->GetID());

	ImGui::Text("Original File: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), res->GetFile());

	ImGui::Text("Exported File: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), res->GetExportedFile().c_str());

	ImGui::Text("Times Loaded: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%d", res->CountReferences());
	
	ImGui::EndChild();
}

void Inspector::DrawComponents(GameObject* obj, bool res)
{
	static bool rename = false;
	position = obj->GetPosition();
	rotation = obj->GetRotation() * RADTODEG;
	scale = obj->GetScale();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Mesh", nullptr, false, !obj->HasComponent(Component::Type::Mesh)))
				obj->AddComponent(Component::Type::Mesh);

			if (ImGui::MenuItem("Material", nullptr, false, !obj->HasComponent(Component::Type::Material)))
				obj->AddComponent(Component::Type::Material);

			if (ImGui::MenuItem("Renderer", nullptr, false, !obj->HasComponent(Component::Type::Renderer)))
				obj->AddComponent(Component::Type::Renderer);

			if (ImGui::MenuItem("Camera", nullptr, false, !obj->HasComponent(Component::Type::Camera)))
				obj->AddComponent(Component::Type::Camera);

			if (ImGui::BeginMenu("UI Element"))
			{
				if (ImGui::MenuItem("Canvas", nullptr, false, !obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS)))
					obj->AddComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);

				if (ImGui::MenuItem("Image", nullptr, false, !obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::IMAGE)))
					obj->AddComponent(Component::Type::UI_Element, UI_Element::Type::IMAGE);

				if (ImGui::MenuItem("Text", nullptr, false, !obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::TEXT)))
					obj->AddComponent(Component::Type::UI_Element, UI_Element::Type::TEXT);

				if (ImGui::MenuItem("Button", nullptr, false, !obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::BUTTON)))
					obj->AddComponent(Component::Type::UI_Element, UI_Element::Type::BUTTON);

				if (ImGui::MenuItem("Check Box", nullptr, false, !obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::CHECKBOX)))
					obj->AddComponent(Component::Type::UI_Element, UI_Element::Type::CHECKBOX);

				if (ImGui::MenuItem("Input Text", nullptr, false, !obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::INPUTTEXT)))
					obj->AddComponent(Component::Type::UI_Element, UI_Element::Type::INPUTTEXT);

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Remove", !obj->components.empty()))
		{
			if (ImGui::MenuItem("Mesh", nullptr, false, obj->HasComponent(Component::Type::Mesh)))
				obj->DeleteComponent(Component::Type::Mesh);

			if (ImGui::MenuItem("Material", nullptr, false, obj->HasComponent(Component::Type::Material)))
				obj->DeleteComponent(Component::Type::Material);

			if (ImGui::MenuItem("Renderer", nullptr, false, obj->HasComponent(Component::Type::Renderer)))
				obj->DeleteComponent(Component::Type::Renderer);

			if (ImGui::MenuItem("Camera", nullptr, false, obj->HasComponent(Component::Type::Camera)))
				obj->DeleteComponent(Component::Type::Camera);

			if (ImGui::BeginMenu("UI Element", obj->HasComponent(Component::Type::UI_Element)))
			{
				if (ImGui::MenuItem("Canvas", nullptr, false, obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS)))
					obj->DeleteComponent(Component::Type::UI_Element, UI_Element::Type::CANVAS);

				if (ImGui::MenuItem("Image", nullptr, false, obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::IMAGE)))
					obj->DeleteComponent(Component::Type::UI_Element, UI_Element::Type::IMAGE);

				if (ImGui::MenuItem("Text", nullptr, false, obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::TEXT)))
					obj->DeleteComponent(Component::Type::UI_Element, UI_Element::Type::TEXT);

				if (ImGui::MenuItem("Button", nullptr, false, obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::BUTTON)))
					obj->DeleteComponent(Component::Type::UI_Element, UI_Element::Type::BUTTON);

				if (ImGui::MenuItem("Check Box", nullptr, false, obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::CHECKBOX)))
					obj->DeleteComponent(Component::Type::UI_Element, UI_Element::Type::CHECKBOX);

				if (ImGui::MenuItem("Input Text", nullptr, false, obj->HasComponent(Component::Type::UI_Element, UI_Element::Type::INPUTTEXT)))
					obj->DeleteComponent(Component::Type::UI_Element, UI_Element::Type::INPUTTEXT);

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (res == true) //Resource Selected
		ImGui::BeginChild("Components", ImVec2(0, ImGui::GetWindowContentRegionMax().y * 0.7f), false, ImGuiWindowFlags_NoTitleBar);

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

		if (ImGui::DragFloat("x##position", &position.x, precision, -inf, inf, precision_char))
			if (!obj->is_static) SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("y##position", &position.y, precision, -inf, inf, precision_char))
			if (!obj->is_static) SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("z##position", &position.z, precision, -inf, inf, precision_char))
			if (!obj->is_static) SetPosition(obj, position);

		ImGui::Separator();

		// Rotation
		ImGui::Text("Rotation");
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("x##rotation", &rotation.x, precision, -inf, inf, precision_char))
			if (!obj->is_static) SetRotation(obj, rotation*DEGTORAD);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("y##rotation", &rotation.y, precision, -inf, inf, precision_char))
			if (!obj->is_static) SetRotation(obj, rotation*DEGTORAD);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("z##rotation", &rotation.z, precision, -inf, inf, precision_char))
			if (!obj->is_static) SetRotation(obj, rotation*DEGTORAD);

		ImGui::Separator();

		// Scale
		ImGui::Text("Scale");
		ImGui::SetNextItemWidth(60);

		if (ImGui::DragFloat("x##scale", &scale.x, precision, -inf, inf, precision_char))
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

		if (ImGui::DragFloat("y##scale", &scale.y, precision, -inf, inf, precision_char))
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

		if (ImGui::DragFloat("z##scale", &scale.z, precision, -inf, inf, precision_char))
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

	if (res == true) //Resource Selected
		ImGui::EndChild();
}