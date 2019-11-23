#include "Quadtree.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResources.h"


Color Quadtree::c = Yellow;

Quadtree::Quadtree(const AABB& box)
{
	root = new QuadtreeNode(box);
	root->level = 0;
	root->tree = this;
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

void QuadtreeNode::DrawEx(Color c)
{
	glColor3ub(c.r * 255.0f, c.g * 255.0f, c.b * 255.0f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabb_IBO);
	glDrawElements(GL_LINES, sizeof(aabb_indices), GL_UNSIGNED_INT, nullptr);
}

void Quadtree::AddGameObject(const GameObject* gameObject)
{
	if (root->AddGameObject(gameObject) == false)
		ExpandRootNode(gameObject);
}

void Quadtree::ExpandRootNode(const GameObject* gameObject)
{
	if (root->box.MinX() > gameObject->aabb.MinX()) root->box.minPoint.x = gameObject->aabb.MinX();
	if (root->box.MinY() > gameObject->aabb.MinY()) root->box.minPoint.y = gameObject->aabb.MinY();
	if (root->box.MinZ() > gameObject->aabb.MinZ()) root->box.minPoint.z = gameObject->aabb.MinZ();

	if (root->box.MaxX() < gameObject->aabb.MaxX()) root->box.maxPoint.x = gameObject->aabb.MaxX();
	if (root->box.MaxY() < gameObject->aabb.MaxY()) root->box.maxPoint.y = gameObject->aabb.MaxY();
	if (root->box.MaxZ() < gameObject->aabb.MaxZ()) root->box.maxPoint.z = gameObject->aabb.MaxZ();

	//root->UpdateVBO();

	std::vector<const GameObject*> all_bucket;
	
	root->GetAllBuckets(all_bucket);

	Clear();

	for (const GameObject* obj : all_bucket)
	{
		AddGameObject(obj);
	}
}


void QuadtreeNode::GetAllBuckets(std::vector<const GameObject*>& all_bucket)
{
	for (QuadtreeNode* child : childs)
	{
		child->GetAllBuckets(all_bucket);
	}

	for (const GameObject* obj : bucket)
	{
		all_bucket.push_back(obj);
	}
}

bool Quadtree::RemoveGameObject(const GameObject* gameObject)
{
	if (root->RemoveGameObject(gameObject) == true)
	{
		return true;
	}
	else
	{
		for (std::vector<const GameObject*>::iterator it = out_of_tree.begin(); it < out_of_tree.end(); it++)
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

void Quadtree::Clear()
{
	root->Clear();
}


// QuadtreeNode -----------------------------------------------------------

QuadtreeNode::QuadtreeNode(const AABB& box) : box(box)
{
	UpdateVBO(box);
}

QuadtreeNode::QuadtreeNode(Quadtree* tree, QuadtreeNode* parent, uint index) : tree(tree)
{
	//Index positions from top view
	//0 1
	//2 3
	float3 minPoint, maxPoint;

	// Y oscillates each index
	minPoint.x = ((index % 2) == 0) ? parent->box.minPoint.x : (parent->box.maxPoint.x + parent->box.minPoint.x) / 2;
	maxPoint.x = ((index % 2) == 0) ? (parent->box.maxPoint.x + parent->box.minPoint.x) / 2 : parent->box.maxPoint.x;

	// Y oscillates every 4 index
	minPoint.y = (index < 4) ? parent->box.minPoint.y : (parent->box.maxPoint.y + parent->box.minPoint.y) / 2;
	maxPoint.y = (index < 4) ? (parent->box.maxPoint.y + parent->box.minPoint.y) / 2 : parent->box.maxPoint.y;

	// Z oscillates every 2 index
	minPoint.z = ((index / 2) % 2 == 0) ? parent->box.minPoint.z : (parent->box.maxPoint.z + parent->box.minPoint.z) / 2;
	maxPoint.z = ((index / 2) % 2 == 0) ? (parent->box.maxPoint.z + parent->box.minPoint.z) / 2 : parent->box.maxPoint.z;


	box = AABB(minPoint, maxPoint);
	box.padding = 0;
	box.padding2 = 0;
	this->level = parent->level+1;

	if (VBO == 0) glGenBuffers(1, &VBO);

	float3 corners[8];
	box.GetCornerPoints(corners);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, box.NumVertices() * sizeof(float3), corners, GL_STATIC_DRAW);
}


QuadtreeNode::~QuadtreeNode()
{
	DeleteVBO();
}

void QuadtreeNode::DeleteVBO()
{
	if (VBO != 0) glDeleteFramebuffers(1, &VBO);
}

void QuadtreeNode::UpdateVBO(const math::AABB & box)
{
	if (VBO == 0) glGenBuffers(1, &VBO);

	float3 corners[8];
	box.GetCornerPoints(corners);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, box.NumVertices() * sizeof(float3), corners, GL_STATIC_DRAW);
}

void QuadtreeNode::Clear()
{
	for (QuadtreeNode* child : childs)
	{
		child->Clear();
	}

	for (QuadtreeNode* child : childs)
	{
		RELEASE(child);
	}

	childs.clear();
	bucket.clear();
}

void QuadtreeNode::Split()
{
	if (!childs.empty()) {
		LOG("[error] Quadtree Node splitting when it already has childs");
	}
	else {
		for (uint i = 0; i < 8; i++)
			childs.push_back(new QuadtreeNode(tree, this, i));
	}
}

bool QuadtreeNode::AddGameObject(const GameObject* gameObject)
{
	if (this->box.Intersects(gameObject->aabb))
	{
		if (childs.empty())
		{
			bucket.push_back(gameObject);
			if (bucket.size() > maxBucketSize)
			{
				Split();
				Redistribute();
			}
		}
		else
		{
			if (!SendToChilds(gameObject))
				bucket.push_back(gameObject);
		}
		return true;
	}
	else
	{
		return false;
	}


}

bool QuadtreeNode::RemoveGameObject(const GameObject* gameObject)
{
	for (std::vector<const GameObject*>::iterator it = bucket.begin(); it != bucket.end(); it++)
	{
		if (*it == gameObject)
		{
			bucket.erase(it);
			TryRemovingChilds();
			return true;
		}
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		if (childs[i]->RemoveGameObject(gameObject) == true)
		{
			TryRemovingChilds();
			return true;
		}

	}
	return false;
}

void QuadtreeNode::Redistribute()
{
	for (std::vector<const GameObject*>::iterator it = bucket.begin(); it != bucket.end();)
	{
		if (SendToChilds(*it))
		{
			it = bucket.erase(it);
		}
		else
		{
			it++;
		}
	}
}
void Quadtree::OptimizeSpace()
{
	root->TryRemovingChilds();


}

bool QuadtreeNode::SendToChilds(const GameObject* gameObject)
{

	for (QuadtreeNode* child : childs)
	{
		if (child->box.Intersects(gameObject->aabb))
		{
			child->AddGameObject(gameObject);
			return true;
		}
	}

	LOG("[error] Quadtree parent node intersecting but not child intersection found", 'e');
	return false;
}

void QuadtreeNode::TryRemovingChilds()
{
	std::vector<const GameObject*> childsBucket;
	GetChildsBuckets(childsBucket, false);
	if (childsBucket.size() + bucket.size() <= maxBucketSize)
	{
		for (uint i = 0; i < childsBucket.size(); i++)
		{
			bucket.push_back(childsBucket[i]);
		}
		childs.clear();
	}
	childsBucket.clear();
}

void QuadtreeNode::GetChildsBuckets(std::vector<const GameObject*>& vector, bool addSelf) const
{
	if (addSelf)
	{
		for (uint i = 0; i < bucket.size(); i++)
		{
			vector.push_back(bucket[i]);
		}
	}

	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->GetChildsBuckets(vector, true);
	}
}

void QuadtreeNode::Draw()
{
	Color color;
	switch (bucket.size())
	{
	case 0:
		color = Color(1, 0, 0, 1);
		break;
	case 1:
		color = Color(0, 1, 0, 1);
		break;
	case 2:
		color = Color(0, 0, 1, 1);
		break;
	case 3:
		color = Color(1, 1, 0, 1);
		break;
	default:
		color = Color(0, 1, 1, 1);
		break;
	}

	/*switch (level)
	{
	case 0:
		color = Color(1, 0, 0, 1);
		break;
	case 1:
		color = Color(0, 1, 0, 1);
		break;
	case 2:
		color = Color(0, 0, 1, 1);
		break;
	case 3:
		color = Color(1, 1, 0, 1);
		break;
	default:
		color = Color(0, 1, 1, 1);
		break;
	}*/

	toDraw = box;
	toDraw.maxPoint.x -= 0.5f;
	toDraw.maxPoint.y -= 0.5f;
	toDraw.maxPoint.z -= 0.5f;
						   
	toDraw.minPoint.x += 0.5f;
	toDraw.minPoint.y += 0.5f;
	toDraw.minPoint.z += 0.5f;

	UpdateVBO(toDraw);
	DrawEx(color);

	for (uint i = 0; i < childs.size(); i++)
		childs[i]->Draw();
}

//bool QuadtreeNode::SendToChilds(const GameObject* gameObject)
//{
//	uint intersectionChild = -1;
//	//ALgo funciona mal aqui, no intersecta amb cap child excepte en un que interescta 4 cops
//	for (uint i = 0; i < childs.size(); i++)
//	{
//		if (childs[i]->box.Intersects(gameObject->aabb))
//		{
//			intersectionChild = i;
//			break;
//		}
//	}
//	if (intersectionChild >= 0 && intersectionChild < childs.size())
//	{
//		childs[intersectionChild]->AddGameObject(gameObject);
//		return true;
//	}
//	else if (intersectionChild == -1)
//		LOG("[error] Quadtree parent node intersecting but not child intersection found",'e');
//	return false;
//}

//bool QuadtreeNode::SendToChilds(const GameObject* gameObject)
//{
//	uint intersectionCount = 0;
//	uint intersectionChild = -1;
//	//ALgo funciona mal aqui, no intersecta amb cap child excepte en un que interescta 4 cops
//	for (uint i = 0; i < childs.size(); i++)
//	{
//		if (childs[i]->box.Intersects(gameObject->aabb))
//		{
//			intersectionCount++;
//			intersectionChild = i;
//		}
//	}
//	if (intersectionCount == 1)
//	{
//		childs[intersectionChild]->AddGameObject(gameObject);
//		return true;
//	}
//	else if (intersectionCount == 0)
//		LOG("[error] Quadtree parent node intersecting but not child intersection found");
//	return false;
//}
