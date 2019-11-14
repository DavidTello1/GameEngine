#include "ResourceModel.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"

#include "Assimp/include/scene.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"


ResourceModel::ResourceModel(UID id) : Resource(id, Resource::Type::model)
{
}

ResourceModel::~ResourceModel()
{
}

void ResourceModel::Save(Config& config) const
{
	Resource::Save(config);
}

void ResourceModel::Load(const Config& config)
{
	Resource::Load(config);
}

bool ResourceModel::Import(const char* full_path, std::string& output)
{
	unsigned flags = aiProcess_CalcTangentSpace | \
		aiProcess_GenSmoothNormals | \
		aiProcess_JoinIdenticalVertices | \
		aiProcess_ImproveCacheLocality | \
		aiProcess_LimitBoneWeights | \
		aiProcess_SplitLargeMeshes | \
		aiProcess_Triangulate | \
		aiProcess_GenUVCoords | \
		aiProcess_SortByPType | \
		aiProcess_FindDegenerates | \
		aiProcess_FindInvalidData |
		0;

	aiString assimp_path(".");
	assimp_path.Append(full_path);

	const aiScene* scene = aiImportFile(assimp_path.data, flags);

	if (scene)
	{
		ResourceModel m(0);

		std::vector<UID> materials, meshes;
		m.GenerateMaterials(scene, full_path, materials);
		m.GenerateMeshes(scene, full_path, meshes);
		m.GenerateNodes(scene, scene->mRootNode, 0, meshes, materials);

		aiReleaseImport(scene);

		return m.SaveOwnFormat(output);
	}
	return false;
}

bool ResourceModel::SaveOwnFormat(std::string& output) const
{
	simple::mem_ostream<std::true_type> write_stream;

	SaveToStream(write_stream);

	const std::vector<char>& data = write_stream.get_internal_vec();

	return App->file_system->SaveUnique(output, &data[0], data.size(), LIBRARY_MODEL_FOLDER, "model", "edumodel");
}
