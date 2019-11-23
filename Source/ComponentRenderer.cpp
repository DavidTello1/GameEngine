#include "Application.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

ComponentRenderer::ComponentRenderer(GameObject* gameobj) : Component(Component::Type::Renderer, gameobj)
{
}

ComponentRenderer::~ComponentRenderer()
{
}

void ComponentRenderer::DrawInspector()
{
	bool active = IsActive();
	if (ImGui::Checkbox("##check3", &active))
		SwitchActive();

	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Renderer"))
	{
		if (object->GetComponent(Component::Type::Mesh) == nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
			ImGui::TextWrapped("No mesh loaded");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::Checkbox("Wireframe", &show_wireframe);
		}
		ImGui::Separator();
	}
}

void ComponentRenderer::Draw()
{
	ComponentMesh* mesh = nullptr;
	for (uint i = 0; i < object->components.size(); i++)
	{
		mesh = (ComponentMesh*)object->components[i];
		if (mesh->GetMesh() != nullptr && mesh->GetType() == Component::Type::Mesh && mesh->IsActive())
		{
			Draw(*mesh, (ComponentMaterial*)mesh->GetGameobj()->GetComponent(Component::Type::Material));

			if (show_face_normals)
				DrawFaceNormals();

			if (show_vertex_normals)
				DrawVertexNormals();
		}
	}
}

void ComponentRenderer::Draw(ComponentMesh& mesh, ComponentMaterial* material) const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	if (show_wireframe || App->scene_base->show_all_wireframe) //wireframe
		glBindTexture(GL_TEXTURE_2D, 0);

	else if (show_checkers) //checkers
		glBindTexture(GL_TEXTURE_2D, App->resources->checkers_texture->tex_id);

	else if (material->GetMaterial() != nullptr && material->GetMaterial()->tex_id != 0 && material->IsActive())
		glBindTexture(GL_TEXTURE_2D, material->GetMaterial()->tex_id);

	else
		glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.GetMesh()->tex_coords_id);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.GetMesh()->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetMesh()->IBO);
	glDrawElements(GL_TRIANGLES, mesh.GetMesh()->num_indices, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ComponentRenderer::DrawFaceNormals()
{
	glBegin(GL_LINES);
	glLineWidth(0.25f);
	glColor3ub(75, 75, 255);

	Triangle face;
	ComponentMesh* mesh = (ComponentMesh*)object->GetComponent(Component::Type::Mesh);

	for (uint i = 0; i < mesh->GetMesh()->num_indices / 3; ++i)
	{
		face.a = mesh->GetMesh()->vertices[mesh->GetMesh()->indices[i * 3]];
		face.b = mesh->GetMesh()->vertices[mesh->GetMesh()->indices[(i * 3) + 1]];
		face.c = mesh->GetMesh()->vertices[mesh->GetMesh()->indices[(i * 3) + 2]];

		float3 center = face.Centroid();
		float3 normal = Cross(face.b - face.a, face.c - face.b);
		normal.Normalize();

		glVertex3f(center.x, center.y, center.z);

		glVertex3f(center.x + normal.x * normals_size,
			center.y + normal.y * normals_size,
			center.z + normal.z * normals_size);
	}

	glColor3ub(255, 255, 255); //reset default color
	glEnd();
}

void ComponentRenderer::DrawVertexNormals()
{
	glBegin(GL_LINES);
	glLineWidth(0.25f);
	glColor3ub(255, 75, 75);

	ComponentMesh* mesh = (ComponentMesh*)object->GetComponent(Component::Type::Mesh);

	if (mesh->GetMesh()->normals)
	{
		for (uint i = 0; i < mesh->GetMesh()->num_indices; ++i)
		{
			glVertex3f(mesh->GetMesh()->vertices[mesh->GetMesh()->indices[i]].x,
				mesh->GetMesh()->vertices[mesh->GetMesh()->indices[i]].y,
				mesh->GetMesh()->vertices[mesh->GetMesh()->indices[i]].z);

			glVertex3f(mesh->GetMesh()->vertices[mesh->GetMesh()->indices[i]].x + mesh->GetMesh()->normals[mesh->GetMesh()->indices[i]].x * normals_size,
				mesh->GetMesh()->vertices[mesh->GetMesh()->indices[i]].y + mesh->GetMesh()->normals[mesh->GetMesh()->indices[i]].y * normals_size,
				mesh->GetMesh()->vertices[mesh->GetMesh()->indices[i]].z + mesh->GetMesh()->normals[mesh->GetMesh()->indices[i]].z * normals_size);
		}
	}
	glColor3ub(255, 255, 255); //reset default color
	glEnd();
}