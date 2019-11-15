#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ResourceModel.h"
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
		ResourceModel model(0);

		std::vector<UID> materials, meshes;
		model.GenerateMaterials(scene, full_path, materials);
		model.GenerateMeshes(scene, full_path, meshes);
		model.CreateGameobjs(scene, scene->mRootNode, 0, meshes, materials);

		aiReleaseImport(scene);

		return model.SaveOwnFormat(output);
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

bool ResourceModel::LoadtoScene()
{
	return true;
}

void ResourceModel::UnLoad()
{
	LOG("UnLoading Model %s with uid %d", name, uid, 'd');
	for (uint i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].mesh != 0)
			App->resources->GetResource(nodes[i].mesh)->Release();

		if (nodes[i].material != 0)
			App->resources->GetResource(nodes[i].material)->Release();
	}
	nodes.clear();
}


void ResourceModel::GenerateMaterials(const aiScene* scene, const char* file, std::vector<UID>& materials)
{
	materials.reserve(scene->mNumMaterials); //reserve capacity for num of materials

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		materials.push_back(ResourceMaterial::Import(scene->mMaterials[i], file)); //import material

		assert(materials.back() != 0); //asssert if vector materials is empty
	}

}

void ResourceModel::GenerateMeshes(const aiScene* scene, const char* file, std::vector<UID>& meshes)
{
	meshes.reserve(scene->mNumMeshes);

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		meshes.push_back(ResourceMesh::Import(scene->mMeshes[i], file));

		assert(meshes.back() != 0);
	}
}

void ResourceModel::CreateGameobjs(const aiScene* model, const aiNode* node, uint parent, const std::vector<UID>& meshes, const std::vector<UID>& materials)
{
	uint index = nodes.size();

	Node dst;

	dst.transform = reinterpret_cast<const float4x4&>(node->mTransformation);
	dst.name = node->mName.C_Str();
	dst.parent = parent;

	nodes.push_back(dst);

	for (unsigned i = 0; i < node->mNumChildren; ++i)
	{
		CreateGameobjs(model, node->mChildren[i], index, meshes, materials);
	}

	if (node->mNumMeshes == 1)
	{
		uint mesh_index = node->mMeshes[0];

		nodes[index].mesh = meshes[mesh_index];
		nodes[index].material = materials[model->mMeshes[mesh_index]->mMaterialIndex];
	}
	else
	{
		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			Node mesh;

			uint mesh_index = node->mMeshes[i];

			mesh.parent = index;
			mesh.mesh = meshes[mesh_index];
			mesh.material = materials[model->mMeshes[mesh_index]->mMaterialIndex];

			char buff[100];

			if (model->mMeshes[mesh_index]->mName.length == 0)
			{
				snprintf(buff, sizeof(buff), "mesh_%d", i);
			}
			else
			{
				snprintf(buff, sizeof(buff), "mesh_%s", model->mMeshes[mesh_index]->mName.C_Str());
			}

			mesh.name = buff;

			nodes.push_back(mesh);
		}
	}
}