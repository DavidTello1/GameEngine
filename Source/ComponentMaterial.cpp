#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(GameObject* gameobj) : Component(Component::Type::Material, gameobj)
{
}

ComponentMaterial::~ComponentMaterial()
{
	bool erase = true;
	for (uint i = 0; i < App->scene->gameObjects.size(); i++)
	{
		ComponentMaterial* material = (ComponentMaterial*)App->scene->gameObjects[i]->GetComponent(Component::Type::Material);
		if (material != nullptr && material != this && material->tex_id == tex_id)
		{
			erase = false;
			break;
		}
	}
	if (erase)
	{
		App->scene->DeleteMaterial(this);
		glDeleteTextures(1, (GLuint*)&tex_id);
	}
}
