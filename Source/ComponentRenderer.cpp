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

void ComponentRenderer::Draw()
{
	ComponentMesh* mesh = nullptr;
	for (uint i = 0; i < object->components.size(); i++)
	{
		mesh = (ComponentMesh*)object->components[i];
		if (mesh->GetType() == Component::Type::Mesh && mesh->IsActive())
		{
			DrawMesh(*mesh);

			if (show_face_normals)
				DrawFaceNormals();

			if (show_vertex_normals)
				DrawVertexNormals();
		}
	}
}

void ComponentRenderer::DrawMesh(ComponentMesh& mesh) const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	ComponentMaterial* material = (ComponentMaterial*)mesh.GetGameobj()->GetComponent(Component::Type::Material);
	if(material != nullptr && material->IsActive())
	{
		if (show_checkers)
			glBindTexture(GL_TEXTURE_2D, App->resources->checker_texture); // start using texture
		else
			glBindTexture(GL_TEXTURE_2D, mesh.TEX);
		
	}
	else
	{
		if (show_checkers)
			glBindTexture(GL_TEXTURE_2D, App->resources->checker_texture);
		else
			glBindTexture(GL_TEXTURE_2D, 0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, mesh.tex_coords_id);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IBO);
	glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);*/
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ComponentRenderer::DrawFaceNormals()
{
	glBegin(GL_LINES);
	glLineWidth(0.25f);
	glColor3ub(75, 75, 255);

	Triangle face;
	ComponentMesh* mesh = (ComponentMesh*)object->GetComponent(Component::Type::Mesh);

	for (uint i = 0; i < mesh->num_indices / 3; ++i)
	{
		face.a = mesh->vertices[mesh->indices[i * 3]];
		face.b = mesh->vertices[mesh->indices[(i * 3) + 1]];
		face.c = mesh->vertices[mesh->indices[(i * 3) + 2]];

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

	if (mesh->normals)
	{
		for (uint i = 0; i < mesh->num_indices; ++i)
		{
			glVertex3f(mesh->vertices[mesh->indices[i]].x, 
					   mesh->vertices[mesh->indices[i]].y, 
					   mesh->vertices[mesh->indices[i]].z);

			glVertex3f(mesh->vertices[mesh->indices[i]].x + mesh->normals[mesh->indices[i]].x * normals_size,
					   mesh->vertices[mesh->indices[i]].y + mesh->normals[mesh->indices[i]].y * normals_size, 
					   mesh->vertices[mesh->indices[i]].z + mesh->normals[mesh->indices[i]].z * normals_size);
		}
	}
	glColor3ub(255, 255, 255); //reset default color
	glEnd();
}
