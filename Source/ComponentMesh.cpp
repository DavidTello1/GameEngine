#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

ComponentMesh::ComponentMesh(GameObject* gameobj) : Component(Component::Type::Mesh, gameobj)
{
	object = gameobj;
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (GetMesh() == nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
			ImGui::TextWrapped("Mesh not loaded");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::Text("Triangles: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", GetMesh()->num_indices / 3);

			ImGui::Text("Vertices: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", GetMesh()->num_vertices);

			ImGui::Text("Vertex Normals: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", GetMesh()->num_normals);

			ImGui::Text("Tex Coords: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", GetMesh()->num_tex_coords);
			ImGui::NewLine();

			ComponentRenderer* renderer = (ComponentRenderer*)object->GetComponent(Component::Type::Renderer);
			if (renderer != nullptr)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
				ImGui::Checkbox("Face Normals", &renderer->show_face_normals);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
				ImGui::Checkbox("Vertex Normals", &renderer->show_vertex_normals);
			}
		}
		ImGui::Separator();
	}
}
