#ifndef __HIERARCHY_H__
#define __HIERARCHY_H__

#include "Panel.h"
#include "Imgui/imgui.h"
#include <random>
#include <set>
#include "HierarchyNode.h"

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
	static void AddNode(HierarchyNode* node = nullptr);
	static void DeleteNode(HierarchyNode * n);
	static bool SetSceneName(const char* name);
	static void DeleteSelected();


	//Data
	static std::set<HierarchyNode*> root_nodes;
	static std::set<HierarchyNode*> nodes;
	static std::set<HierarchyNode*> selected_nodes;

	static char scene_name[MAX_NAME_LENGTH];
};

#endif// __HIERARCHY_H__
