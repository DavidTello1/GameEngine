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

	static Color c;

private:
	QuadtreeNode* root = nullptr;

	GameObject* min = nullptr;
	GameObject* max = nullptr;

	std::vector<GameObject*> out_of_tree;
};
