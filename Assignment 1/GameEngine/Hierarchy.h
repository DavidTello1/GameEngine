#pragma once
#include <vector>
#include <random>
#include <set>
#include "HierarchyNode.h"




class Hierarchy
{

public:

	Hierarchy();
	~Hierarchy();

	static void AddNode(HierarchyNode* node = nullptr);

	//static Hierarchy hierarchy;
	static void Init();

	//Gui

	static void DeleteSelected();

	static void ShowHierarchy(bool* open);

	static void Draw(const char* title, bool* p_open);

	//Naming

	static bool SetSceneName(const char* name);

	static char scene_name[MAX_NAME_LENGTH];

	//Data

	static std::set<HierarchyNode*> root_nodes;
	static std::set<HierarchyNode*> nodes;

	static std::set<HierarchyNode*> selected_nodes;


};

