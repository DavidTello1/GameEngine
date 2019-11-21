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
}
