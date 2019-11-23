#pragma once

#include <vector>
#include "Math.h"
#include <map>

typedef unsigned int uint;

class GameObject;
class QuadtreeNode;
class Color;

class Quadtree
{
public:

	Quadtree(const AABB& box);
	~Quadtree();

	void Draw();

	void AddGameObject(const GameObject* gameObject);
	void ExpandRootNode(const GameObject * gameObject);
	bool RemoveGameObject(const GameObject* gameObject);

	void Clear();

	void OptimizeSpace();

	template<typename PRIMITIVE>
	void CollectCandidates(std::vector<const GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		root->CollectCandidates(gameObjects, primitive);
	}

	static Color c;

private:
	QuadtreeNode* root;
	std::vector<const GameObject*> out_of_tree;
};



class QuadtreeNode
{
	friend class Quadtree;

public:

	// TO IMPROVE
	void Draw();
	void DrawEx(Color c);

	void GetAllBuckets(std::vector<const GameObject*>& all_bucket);

	QuadtreeNode(const AABB& box);
	QuadtreeNode(Quadtree* tree, QuadtreeNode* parent, uint index);	//Index marking which node from parent. 0 starts at top left, and counting clockwise

	~QuadtreeNode();

	void UpdateVBO(const math::AABB & box);
	void Clear();
	void DeleteVBO();

	bool AddGameObject(const GameObject* gameObject);
	bool RemoveGameObject(const GameObject* gameObject);

	template<typename PRIMITIVE>
	void CollectCandidates(std::vector<const GameObject*>& gameObjects, const PRIMITIVE& primitive);

private:
	//void Draw();

	void Split();
	void Redistribute();

	bool SendToChilds(const GameObject* gameObject);
	void TryRemovingChilds();
	void GetChildsBuckets(std::vector<const GameObject*>& vector, bool addSelf) const;

private:

	AABB box;
	AABB toDraw;
	uint VBO = 0;
	std::vector<QuadtreeNode*> childs;

	//Pointer to tree, maybe not necessary
	Quadtree* tree;
	uint level;
	uint maxBucketSize = 3;
	std::vector<const GameObject*> bucket;

	//uint index = 0;
};

template<typename PRIMITIVE>
void QuadtreeNode::CollectCandidates(std::vector<const GameObject*>& gameObjects, const PRIMITIVE& primitive)
{
	if (primitive.Intersects(box))
	{
		for (uint i = 0; i < bucket.size(); i++)
		{
			gameObjects.push_back(bucket[i]);
		}

		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i].CollectCandidates(gameObjects, primitive);
		}
	}
}
