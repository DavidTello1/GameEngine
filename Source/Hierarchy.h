#ifndef __HIERARCHY_H__
#define __HIERARCHY_H__

#include "Panel.h"
#include "HierarchyNode.h"
#include "Imgui/imgui.h"
#include <random>
#include <vector>

class Hierarchy : public Panel
{
public:
	static const uint default_width = 262;
	static const uint default_height = 862;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 19;

public:
	Hierarchy();
	virtual ~Hierarchy();

	void Draw();

	static HierarchyNode* AddNode(GameObject* object, GameObject* parent = nullptr);
	static void ChildAdded(GameObject* child, HierarchyNode * to);
	static void DeleteNode(HierarchyNode * n);
	static void DeleteSelected();
	static bool SearchAndDeleteNode(HierarchyNode* n, std::vector<HierarchyNode*>& v);

	void UnSelectAll(HierarchyNode* keep_selected = nullptr);

	void SetSceneName(const char* name);

	HierarchyNode * SearchById(const uint id);

	static void GetMinMaxVertex(const HierarchyNode* node, float3 * abs_min, float3 * abs_max);

private:
	void DrawCreateMenu();
	void DrawNodes(std::vector<HierarchyNode*>& v);

public:
	//Data
	static GameObject* root_node;

	static std::vector<GameObject*> nodes;

	char scene_name[NAME_LENGTH];
};

#endif// __HIERARCHY_H__
