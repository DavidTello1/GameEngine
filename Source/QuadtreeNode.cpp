#include "QuadtreeNode.h"
#include "GameObject.h"

#include "ModuleResources.h"

void QuadtreeNode::DrawEx(Color c)
{
	if (!is_culling)
		glColor3ub(c.r * 255.0f, c.g * 255.0f, c.b * 255.0f);
	else
		glColor3ub(culling_color.r * 255.0f, culling_color.g * 255.0f, culling_color.b * 255.0f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabb_IBO);
	glDrawElements(GL_LINES, sizeof(aabb_indices), GL_UNSIGNED_INT, nullptr);
}

void QuadtreeNode::ResetCullingState()
{
	for (QuadtreeNode* child : childs)
	{
		child->ResetCullingState();
	}

	for (GameObject* b : bucket)
	{
		b->is_drawn = true;
	}

	is_culling = false;
}

void QuadtreeNode::GetAllBuckets(std::vector<GameObject*>& all_bucket)
{
	for (QuadtreeNode* child : childs)
	{
		child->GetAllBuckets(all_bucket);
	}

	for (GameObject* obj : bucket)
	{
		all_bucket.push_back(obj);
	}
}


QuadtreeNode::QuadtreeNode(const AABB& box) : box(box)
{
	UpdateVBO(box);
}

QuadtreeNode::QuadtreeNode(Quadtree* tree, QuadtreeNode* parent, uint index) : tree(tree)
{
	float3 minPoint, maxPoint;

	if (QUADTREE == 4)
	{
		// X oscillates each index
		minPoint.x = ((index % 2) == 0) ? parent->box.minPoint.x : (parent->box.maxPoint.x + parent->box.minPoint.x) / 2;
		maxPoint.x = ((index % 2) == 0) ? (parent->box.maxPoint.x + parent->box.minPoint.x) / 2 : parent->box.maxPoint.x;

		// Y Do not oscillates
		minPoint.z = parent->box.minPoint.z;
		maxPoint.z = parent->box.maxPoint.z;

		// Z oscillates every 2 index
		minPoint.y = ((index / 2) % 2 == 0) ? parent->box.minPoint.y : (parent->box.maxPoint.y + parent->box.minPoint.y) / 2;
		maxPoint.y = ((index / 2) % 2 == 0) ? (parent->box.maxPoint.y + parent->box.minPoint.y) / 2 : parent->box.maxPoint.y;

	}
	else if (QUADTREE == 8)
	{
		// Y oscillates each index
		minPoint.x = ((index % 2) == 0) ? parent->box.minPoint.x : (parent->box.maxPoint.x + parent->box.minPoint.x) / 2;
		maxPoint.x = ((index % 2) == 0) ? (parent->box.maxPoint.x + parent->box.minPoint.x) / 2 : parent->box.maxPoint.x;

		// Y oscillates every 4 index
		minPoint.y = (index < 4) ? parent->box.minPoint.y : (parent->box.maxPoint.y + parent->box.minPoint.y) / 2;
		maxPoint.y = (index < 4) ? (parent->box.maxPoint.y + parent->box.minPoint.y) / 2 : parent->box.maxPoint.y;

		// Z oscillates every 2 index
		minPoint.z = ((index / 2) % 2 == 0) ? parent->box.minPoint.z : (parent->box.maxPoint.z + parent->box.minPoint.z) / 2;
		maxPoint.z = ((index / 2) % 2 == 0) ? (parent->box.maxPoint.z + parent->box.minPoint.z) / 2 : parent->box.maxPoint.z;

	}
	box = AABB(minPoint, maxPoint);
	box.padding = 0;
	box.padding2 = 0;
	this->level = parent->level + 1;

	UpdateVBO(box);
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

	//float3 corners[8];
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
		for (uint i = 0; i < QUADTREE; i++)
			childs.push_back(new QuadtreeNode(tree, this, i));
	}
}

bool QuadtreeNode::AddGameObject(GameObject* gameObject)
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

bool QuadtreeNode::RemoveGameObject(GameObject* gameObject)
{
	for (std::vector<GameObject*>::iterator it = bucket.begin(); it != bucket.end(); it++)
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
	for (std::vector<GameObject*>::iterator it = bucket.begin(); it != bucket.end();)
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


bool QuadtreeNode::SendToChilds(GameObject* gameObject)
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
	std::vector<GameObject*> childsBucket;
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

void QuadtreeNode::GetChildsBuckets(std::vector<GameObject*>& vector, bool addSelf) const
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

//bool QuadtreeNode::SendToChilds(GameObject* gameObject)
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

//bool QuadtreeNode::SendToChilds(GameObject* gameObject)
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
