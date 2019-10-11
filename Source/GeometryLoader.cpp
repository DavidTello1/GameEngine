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

MeshData* GeometryLoader::LoadModel(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	MeshData* m = new MeshData();
	if (scene != nullptr && scene->HasMeshes()) 
	{   // Use scene->mNumMeshes to iterate on scene->mMeshes array   	
		
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* new_mesh = scene->mMeshes[i];

			// copy vertices 
			m->num_vertex = new_mesh->mNumVertices; 
			m->vertex = new float[m->num_vertex * 3]; 
			memcpy(m->vertex, new_mesh->mVertices, sizeof(float) * m->num_vertex * 3); 

			LOG("New mesh with %d vertices", m->num_vertex,'g');

			// copy faces
			if (new_mesh->HasFaces()) 
			{  
				m->num_index = new_mesh->mNumFaces * 3;  
				m->index = new uint[m->num_index]; 
				// assume each face is a triangle  
				for (uint i = 0; i < new_mesh->mNumFaces; ++i)  
				{   
					if (new_mesh->mFaces[i].mNumIndices != 3) {

						LOG("WARNING, geometry face with != 3 indices!",'g');   
					}
					else
					{
						memcpy(&m->index[i*3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));  
					}
				} 
			}

		}
		aiReleaseImport(scene); 
		return m;
	} 
	else
	{
		LOG("Error loading scene %s", path,'e');
	}

	return nullptr;
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
