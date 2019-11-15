#pragma once
#include "Resource.h"
#include "Math.h"

struct aiScene;
struct aiNode;

class ResourceModel : public Resource
{
public:
	struct Node
	{
		std::string name;
		float4x4 transform = float4x4::identity;
		uint parent = 0;
		UID mesh = 0;
		UID material = 0;
	};

public:
	ResourceModel(UID id);
	virtual ~ResourceModel();

	void Save(Config& config) const override;
	void Load(const Config& config) override;
		 
	static bool Import(const char* full_path, std::string& output);
	bool SaveOwnFormat(std::string& output) const;
	bool LoadtoScene();
	void UnLoad();

	unsigned GetNumNodes() const { return nodes.size(); }
	const Node& GetNode(uint index) const { return nodes[index]; }

private:
	void GenerateMaterials(const aiScene* scene, const char* file, std::vector<UID>& materials);
	void GenerateMeshes(const aiScene* scene, const char* file, std::vector<UID>& meshes);
	void CreateGameobjs(const aiScene* model, const aiNode* node, uint parent, const std::vector<UID>& meshes, const std::vector<UID>& materials);

private:
	std::vector<Node> nodes;

};
