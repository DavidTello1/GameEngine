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

	MeshData m;
	if (scene != nullptr && scene->HasMeshes()) 
	{   // Use scene->mNumMeshes to iterate on scene->mMeshes array   	
		
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* new_mesh = scene->mMeshes[i];
			// copy vertices 
			m.num_vertices = new_mesh->mNumVertices; 
			m.vertices = new float[m.num_vertices * 3]; 
			memcpy(m.vertices, new_mesh->mVertices, sizeof(float) * m.num_vertices * 3); 
			LOG("New mesh with %d vertices", m.num_vertices);

			/*m.num_vertices = new_mesh->mNumVertices; 
			m.vertices = new float[m.num_vertices * 3]; 
			memcpy(m.vertices, new_mesh->mVertices, sizeof(float) * m.num_vertices * 3); 
			LOG("New mesh with %d vertices", m.num_vertices);*/
		}
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
