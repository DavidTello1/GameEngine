#include "ComponentMaterial.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(GameObject* gameobj) : Component(Component::Type::Material, gameobj)
{
}

ComponentMaterial::~ComponentMaterial()
{
	glDeleteTextures(1, (GLuint*)&tex_id);
}
