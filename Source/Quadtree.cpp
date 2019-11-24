#include "Quadtree.h"

#include "Application.h"

int Quadtree::depth = 0;
Quadtree::Quadtree(const AABB& box)
{
	CreateRoot(box);
}

void Quadtree::CreateRoot(const AABB& box)
{
	RELEASE(root);

	root = new QuadtreeNode(box);
	root->level = 0;
	root->tree = this;
	depth = 0;
	min = { 0,0,0 };
	max = { 0,0,0 };
}

Quadtree::~Quadtree()
{
	RELEASE(root);
	Clear();
}

void Quadtree::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glLineWidth(1.0f);

	root->Draw();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glColor3ub(255, 255, 255);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Quadtree::AddGameObject(GameObject* gameObject)
{
	if (root->AddGameObject(gameObject) == false)
		if (experimental) ExpandRootNode(gameObject);
}
bool Quadtree::RemoveGameObject(GameObject* gameObject)
{
	if (root->RemoveGameObject(gameObject) == true)
	{
		return true;
	}
	else
	{
		for (std::vector<GameObject*>::iterator it = out_of_tree.begin(); it < out_of_tree.end(); it++)
		{
			if (*it == gameObject)
			{
				out_of_tree.erase(it);
				return true;
			}
		}
		return false;
	}
}
void Quadtree::ExpandRootNode(GameObject* gameObject)
{
	return;
	float3 min = root->box.minPoint;
	float3 max = root->box.maxPoint;

	if (min.x > gameObject->aabb.MinX()) min.x = gameObject->aabb.MinX();
	if (min.y > gameObject->aabb.MinY()) min.y = gameObject->aabb.MinY();
	if (min.z > gameObject->aabb.MinZ()) min.z = gameObject->aabb.MinZ();

	if (max.x < gameObject->aabb.MaxX()) max.x = gameObject->aabb.MaxX();
	if (max.y < gameObject->aabb.MaxY()) max.y = gameObject->aabb.MaxY();
	if (max.z < gameObject->aabb.MaxZ()) max.z = gameObject->aabb.MaxZ();

	//root->UpdateVBO();

	std::vector<GameObject*> all_bucket;
	
	root->GetAllBuckets(all_bucket);


	Clear();
	RELEASE(root);
	CreateRoot(AABB(min,max));

	for (GameObject* obj : all_bucket)
	{
		AddGameObject(obj);
	}
}


void Quadtree::OptimizeSpace()
{
	root->TryRemovingChilds();
}

void Quadtree::ResetCullingState()
{
	root->ResetCullingState();
}

void Quadtree::Clear()
{
	root->Clear();
}
