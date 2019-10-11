#include "GeometryLoader.h"



GeometryLoader::GeometryLoader(bool start_enabled) : Module("GeometryLoader", start_enabled)
{
}


GeometryLoader::~GeometryLoader()
{
}

bool GeometryLoader::Start()
{
	// Stream log messages to Debug window 
	struct aiLogStream stream; 
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr); 
	aiAttachLogStream(&stream);

	return true;
}

bool GeometryLoader::LoadModel(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) 
	{   // Use scene->mNumMeshes to iterate on scene->mMeshes array   
		aiReleaseImport(scene); 
	} 
	else
	{
		LOG("Error loading scene %s", path,'g');
	}
}

bool GeometryLoader::Update(float dt)
{
	
	return true;
}

bool GeometryLoader::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}
