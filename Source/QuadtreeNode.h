#pragma once

#include <vector>
#include <map>
#include "Math.h"
#include "Color.h"

typedef unsigned int uint;

class GameObject;
class QuadtreeNode;
class Color;

class QuadtreeNode
{
	friend class Quadtree;

public:
	static bool is_quadtree;
	static int QUADTREE;
	// TO IMPROVE
	void Draw();
	void DrawEx(Color c);

	void ResetCullingState();

	void GetAllBuckets(std::vector<GameObject*>& all_bucket);

	QuadtreeNode(const AABB& box);
	QuadtreeNode(Quadtree* tree, QuadtreeNode* parent, uint index);	//Index marking which node from parent. 0 starts at top left, and counting clockwise

	~QuadtreeNode();

	void UpdateVBO(const math::AABB & box);
	void Clear();
	void DeleteVBO();

	bool AddGameObject(GameObject* gameObject);
	bool RemoveGameObject(GameObject* gameObject);

	template<typename PRIMITIVE>
	void CollectCandidates(std::vector< GameObject*>& gameObjects, const PRIMITIVE& primitive);

	template<typename PRIMITIVE>
	void CollectCandidates(std::map<float, GameObject*>& gameObjects, const PRIMITIVE& primitive);

private:
	//void Draw();

	void Split();
	void Redistribute();

	bool SendToChilds(GameObject* gameObject);
	void TryRemovingChilds();
	void GetChildsBuckets(std::vector<GameObject*>& vector, bool addSelf) const;

private:

	AABB box;
	AABB toDraw;
	uint VBO = 0;
	std::vector<QuadtreeNode*> childs;

	//Pointer to tree, maybe not necessary
	Quadtree* tree;
	uint level;
	uint maxBucketSize = 8;
	std::vector<GameObject*> bucket;

	float3 corners[8];
	Color culling_color = LightGrey;
	bool is_culling = false;
};

template<typename PRIMITIVE>
void QuadtreeNode::CollectCandidates(std::vector<GameObject*>& gameObjects, const PRIMITIVE& primitive)
{
	if (primitive.DO_Intersects(box))
	{
		for (uint i = 0; i < bucket.size(); i++)
		{
			gameObjects.push_back(bucket[i]);
		}

		for (uint i = 0; i < childs.size(); i++)
		{
			if (!(childs[i]->childs.empty() && childs[i]->bucket.empty()))
				childs[i]->CollectCandidates(gameObjects, primitive);
			else
				childs[i]->is_culling = true;
		}

		is_culling = false;
	}
	else {
		// Skips drawing
		is_culling = true;
	}
}

template<typename PRIMITIVE>
void QuadtreeNode::CollectCandidates(std::map<float, GameObject*>& gameObjects, const PRIMITIVE& primitive)
{
	if (primitive.Intersects(box))
	{
		float hit_near, hit_far;
		for (uint i = 0; i < bucket.size(); i++)
		{
			if (primitive.Intersects(bucket[i]->GetOBB(), hit_near, hit_far))
				gameObjects[hit_near] = bucket[i];
		}
		for (uint i = 0; i < childs.size(); i++)
		{
			if (!(childs[i]->childs.empty() && childs[i]->bucket.empty()))
				childs[i]->CollectCandidates(gameObjects, primitive);
		}
	}
}