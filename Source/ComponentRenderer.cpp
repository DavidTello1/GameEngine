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
			DrawMesh(*mesh);
	}
}

void ComponentRenderer::DrawMesh(ComponentMesh& mesh) const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnable(GL_TEXTURE_2D);
	//glActiveTexture(GL_TEXTURE0);

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
	
	

	if (show_bounding_box && mesh.bb_VBO != 0)
	{
		glColor3ub(255, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.bb_VBO);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.bb_IBO);
		glDrawElements(GL_LINES, sizeof(mesh.bbox_indices), GL_UNSIGNED_INT, nullptr);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glColor3ub(255, 255, 255);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
