#include "Resource.h"
#include "Config.h"
#include "Application.h"
#include "ModuleFileSystem.h"

Resource::Resource(UID Uid, Resource::Type Type) : uid(Uid), type(Type)
{
}

Resource::~Resource()
{
}

bool Resource::LoadToMemory()
{
	if (times_loaded > 0)
		times_loaded++;
	else
		times_loaded = LoadtoScene() ? 1 : 0;

	return times_loaded > 0;
}

void Resource::ReleaseFromMemory()
{
	if (times_loaded > 0)
		times_loaded--;

	if (times_loaded == 0)
		UnLoad();
}

std::string Resource::GetExportedFile() const
{
	std::string file(std::to_string(uid));

	switch (type)
	{
	case unknown:
		break;
	case model:
		file += ".dvs_model";
		break;
	case mesh:
		file += ".dvs_mesh";
		break;
	case material:
		file += ".dvs_material";
		break;
	case scene:
		file += ".dvs_scene";
		break;
	}

	return file;
}