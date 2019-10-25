#include "Component.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

Component::Component(Component::Type Type, GameObject* Object)
{
	if (Object != nullptr)
		object = Object;

	type = Type;
	Enable();
}
