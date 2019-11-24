#pragma once

#include "QuadtreeNode.h"
#include <map>


class GameObject;
class QuadtreeNode;
class Color;



class Quadtree
{

public:

	Quadtree(const AABB& box);
	~Quadtree();

	void CreateRoot(const AABB & box);

	void Draw();

	void AddGameObject(GameObject* gameObject);
	void ExpandRootNode(GameObject * gameObject);
	bool RemoveGameObject(GameObject* gameObject);

	void Clear();

	void OptimizeSpace();

	void ResetCullingState();

	template<typename PRIMITIVE>
	void CollectCandidates(std::vector<GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		root->CollectCandidates(gameObjects, primitive);
	}
	template<typename PRIMITIVE>
	void CollectCandidates(std::map<float, GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		root->CollectCandidates(gameObjects, primitive);
	}
	float3 GetMinPoint(){return root->box.minPoint;}
	float3 GetMaxPoint(){return root->box.maxPoint;}
	static int depth;
	bool experimental = false;

private:
	QuadtreeNode* root = nullptr;

	float3 min = {0,0,0};
	float3 max = {0,0,0};

	std::vector<GameObject*> out_of_tree;
};
