#include "Quadtree.h"
#include "ComponentMesh.h"
#include "Application.h"

int Quadtree::depth = 0;
bool Quadtree::bucket_depth = true;
float3 Quadtree::min_point = { -50,-50,-50 };
float3 Quadtree::max_point = { 50,50,50 };

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
	/*min = { 0,0,0 };
	max = { 0,0,0 };*/
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
	if (IsExcluded(gameObject))
		return;
	if (root->AddGameObject(gameObject) == false)
		if (experimental && NeedsExpansion(gameObject))
			ExpandRootNode(gameObject);
}

bool Quadtree::IsExcluded(GameObject* gameObject)
{
	if (gameObject->GetComponent<ComponentCamera>() ||
		!gameObject->GetComponent<ComponentMesh>())
		return true;

	return false;
}
bool Quadtree::NeedsExpansion(GameObject* gameObject)
{
	bool expand = false;

	if (root->box.minPoint.x > gameObject->aabb.MinX()) { root->box.minPoint.x = gameObject->aabb.MinX(); expand = true;}
	if (root->box.minPoint.y > gameObject->aabb.MinY()) { root->box.minPoint.y = gameObject->aabb.MinY(); expand = true;}
	if (root->box.minPoint.z > gameObject->aabb.MinZ()) { root->box.minPoint.z = gameObject->aabb.MinZ(); expand = true;}
	
	if (root->box.maxPoint.x < gameObject->aabb.MaxX()) { root->box.maxPoint.x = gameObject->aabb.MaxX(); expand = true;}
	if (root->box.maxPoint.y < gameObject->aabb.MaxY()) { root->box.maxPoint.y = gameObject->aabb.MaxY(); expand = true;}
	if (root->box.maxPoint.z < gameObject->aabb.MaxZ()) { root->box.maxPoint.z = gameObject->aabb.MaxZ(); expand = true;}

	return expand;
}
bool Quadtree::RemoveGameObject(GameObject* gameObject)
{
	if (root->RemoveGameObject(gameObject) == true)
	{
		return true;
	}
	return false;
}
void Quadtree::ExpandRootNode(GameObject* gameObject)
{
	root->UpdateVBO(root->box);
	AddGameObject(gameObject);
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
	root->box.minPoint = min_point;
	root->box.maxPoint = max_point;
}
