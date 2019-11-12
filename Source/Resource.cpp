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
