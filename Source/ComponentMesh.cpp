#include "ComponentMesh.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh(GameObject* gameobj) : Component(Component::Type::Mesh, gameobj)
{
	object = gameobj;

}

ComponentMesh::~ComponentMesh()
{
}