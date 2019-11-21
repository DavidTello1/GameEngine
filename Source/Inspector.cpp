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
	static bool has_mesh = false;
	static bool has_material = false;
	static bool has_renderer = false;
	static bool has_camera = false;
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

	if (has_mesh = obj->HasComponent(Component::Type::Mesh))
		mesh = (ComponentMesh*)obj->GetComponent(Component::Type::Mesh);

	if (has_material = obj->HasComponent(Component::Type::Material))
		material = (ComponentMaterial*)obj->GetComponent(Component::Type::Material);

	if (has_renderer = obj->HasComponent(Component::Type::Renderer))
		renderer = (ComponentRenderer*)obj->GetComponent(Component::Type::Renderer);

	if (has_camera = obj->HasComponent(Component::Type::Camera))
		camera = (ComponentCamera*)obj->GetComponent(Component::Type::Camera);

	if (ImGui::BeginMenuBar())
	{
		in_menu = false;

		if (ImGui::BeginMenu("Add"))
		{
			in_menu = true;

			if (ImGui::MenuItem("Mesh", nullptr, false, !has_mesh))
				obj->AddComponent(Component::Type::Mesh);

			if (ImGui::MenuItem("Material", nullptr, false, !has_material))
				obj->AddComponent(Component::Type::Material);

			if (ImGui::MenuItem("Renderer", nullptr, false, !has_renderer))
				obj->AddComponent(Component::Type::Renderer);

			if (ImGui::MenuItem("Camera", nullptr, false, !has_camera))
			{
				obj->AddComponent(Component::Type::Camera);
			}

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Remove", !obj->components.empty()))
		{
			in_menu = true;

			if (has_mesh)
			{
				if (ImGui::MenuItem("Mesh"))
					obj->DeleteComponent(Component::Type::Mesh);
			}
			if (has_material)
			{
				if (ImGui::MenuItem("Material"))
					obj->DeleteComponent(Component::Type::Material);
			}
			if (has_renderer)
			{
				if (ImGui::MenuItem("Renderer"))
					obj->DeleteComponent(Component::Type::Renderer);
			}
			if (has_camera)
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
	}

	if (has_mesh)
	{
		const ResourceMesh* r_mesh = mesh->GetMesh(); //get mesh resource

		bool active = mesh->IsActive();
		if (ImGui::Checkbox("##check", &active))
			mesh->SwitchActive();

		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Mesh"))
		{
			ImGui::Text("Triangles: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)(r_mesh->num_indices / 3));

			ImGui::Text("Vertices: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)(r_mesh->num_vertices));

			ImGui::Text("Vertex Normals: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)(r_mesh->num_normals));

			ImGui::Text("Tex Coords: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)(r_mesh->num_tex_coords));
			ImGui::NewLine();

			if (has_renderer)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
				ImGui::Checkbox("Face Normals", &renderer->show_face_normals);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
				ImGui::Checkbox("Vertex Normals", &renderer->show_vertex_normals);
			}
			ImGui::Separator();
		}
	}

	if (has_material)
	{
		bool active = material->IsActive();
		if (ImGui::Checkbox("##check2", &active))
			material->SwitchActive();

		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Material"))
		{
			if (material->GetMaterial()->tex_id == NULL)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
				ImGui::TextWrapped("Material not loaded");
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::Text("Path: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", material->GetMaterial()->GetFile());

				ImGui::Text("Size: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", material->GetMaterial()->tex_width);
				ImGui::SameLine();
				ImGui::Text("x");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", material->GetMaterial()->tex_width);

				if (ImGui::TreeNode("Image"))
				{
					ImGui::Image((ImTextureID)material->GetMaterial()->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::TreePop();
				}
			}

			ImGui::NewLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::Checkbox("Checkers Material", &renderer->show_checkers);
			ImGui::Separator();
		}
	}

	if (has_renderer)
	{
		bool active = renderer->IsActive();
		if (ImGui::Checkbox("##check3", &active))
			renderer->SwitchActive();
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Renderer"))
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::Checkbox("BoundingBox", &obj->show_bounding_box);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::Checkbox("Wireframe", &renderer->show_wireframe);
			ImGui::Separator();
		}
	}
}
