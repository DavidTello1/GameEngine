#include "Inspector.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "GameObject.h"

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
	static bool has_mesh = false;
	static bool has_material = false;
	static bool has_renderer = false;

	obj = App->scene->GetSelectedGameobj();
	if (obj == nullptr)
	{
		position = rotation = scale = float3::zero;
		return;
	}
	position = obj->GetPosition();
	rotation = obj->GetRotation();
	scale = obj->GetScale();

	has_mesh = obj->HasComponent(Component::Type::Mesh);
	has_material = obj->HasComponent(Component::Type::Material);
	has_renderer = obj->HasComponent(Component::Type::Renderer);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add Component"))
		{
			in_menu = true;

			if (ImGui::MenuItem("Mesh", nullptr, false, !has_mesh))
				obj->AddComponent(Component::Type::Mesh);

			if (ImGui::MenuItem("Material", nullptr, false, !has_material))
				obj->AddComponent(Component::Type::Material);

			if (ImGui::MenuItem("Renderer", nullptr, false, !has_renderer))
				obj->AddComponent(Component::Type::Renderer);

			if (ImGui::MenuItem("Light", nullptr, false, false))
			{
				//obj->AddComponent(Component::Type::Light);
			}

			ImGui::EndMenu();
		}
		else
			in_menu = false;

		ImGui::EndMenuBar();
	}

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Position
		ImGui::Text("Position");
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("x##1", &position.x))
			SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("y##1", &position.y))
			SetPosition(obj, position);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("z##1", &position.z))
			SetPosition(obj, position);

		ImGui::Separator();

		// Rotation
		ImGui::Text("Rotation");
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("x##2", &rotation.x))
			SetRotation(obj, rotation);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("y##2", &rotation.y))
			SetRotation(obj, rotation);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("z##2", &rotation.z))
			SetRotation(obj, rotation);

		ImGui::Separator();

		// Scale
		ImGui::Text("Scale");
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("x##3", &scale.x))
			SetScale(obj, scale);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("y##3", &scale.y))
			SetScale(obj, scale);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::InputFloat("z##3", &scale.z))
			SetScale(obj, scale);

		//ImGui::Separator();
		//ImGui::Text("Advanced");
		//ImGui::Text("Triangle count: %d", 9564);
	}

	if (obj->GetComponent(Component::Type::Mesh))
	{
		if (ImGui::CollapsingHeader("Mesh"))
		{
		}
	}

	if (obj->GetComponent(Component::Type::Material))
	{
		if (ImGui::CollapsingHeader("Material"))
		{
		}
	}

	if (obj->GetComponent(Component::Type::Renderer))
	{
		if (ImGui::CollapsingHeader("Renderer"))
		{
		}
	}
}