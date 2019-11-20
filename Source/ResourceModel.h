#pragma once
#include "Resource.h"

struct aiScene;
struct aiNode;

class ResourceModel : public Resource
{
public:
	struct Node
	{
		std::string name;
		uint parent = 0;
		UID mesh = 0;
		UID material = 0;
	};

public:
	ResourceModel(UID id);
	virtual ~ResourceModel();

	//void Save(Config& config) const override;
	//void Load(const Config& config) override;
		 
	static bool Import(const char* full_path, std::string& output);
	bool SaveOwnFormat(std::string& output) const;
	bool LoadtoScene();
	void UnLoad();

	unsigned GetNumNodes() const { return nodes.size(); }
	const Node& GetNode(uint index) const { return nodes[index]; }

private:
	void CreateNodes(const aiScene* model, const aiNode* node, uint parent, const std::vector<UID>& meshes, const std::vector<UID>& materials);
	void CreateGameObjects(const char* name = nullptr);

private:
	std::vector<Node> nodes;
};
