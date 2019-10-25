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
	HierarchyNode * SearchById(const uint id);
	virtual ~Hierarchy();

	void Draw();

	static HierarchyNode* AddNode(GameObject* object = nullptr, HierarchyNode* node = root_node);
	static void DeleteNode(HierarchyNode * n);
	static void DeleteSelected();
	static bool SearchAndDeleteNode(HierarchyNode* n, std::vector<HierarchyNode*>& v);

	void UnSelectAll(HierarchyNode* keep_selected = nullptr);

	void SetSceneName(const char* name);

private:
	void DrawCreateMenu();
	void DrawNodes(std::vector<HierarchyNode*>& v);

public:
	//Data
	static HierarchyNode* root_node;
	static std::vector<HierarchyNode*> nodes;
	static std::vector<HierarchyNode*> selected_nodes;

	const char* scene_name;
};

#endif// __HIERARCHY_H__
