#include "Resource.h"
#include "Config.h"
#include "Application.h"
#include "ModuleFileSystem.h"

Resource::Resource(UID uid, Resource::Type type) : uid(uid), type(type)
{
}

Resource::~Resource()
{
}

bool Resource::LoadToMemory()
{
	if (loaded > 0)
		loaded++;
	else
		loaded = LoadtoScene() ? 1 : 0;

	return loaded > 0;
}

void Resource::Release()
{
	if (--loaded == 0)
		UnLoad();
}
