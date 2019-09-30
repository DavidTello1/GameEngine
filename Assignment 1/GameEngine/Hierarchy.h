#pragma once
#include <vector>
#include <random>
#include <set>
#include "ImGui/imgui.h"

#define MAX_NAME_LENGTH 512

struct HierarchyNode 
{

	long id;
	bool is_selected;
	char name[MAX_NAME_LENGTH];
	std::vector<HierarchyNode*> childs;
	ImGuiTreeNodeFlags flags;

	HierarchyNode() 
	{
		id = lrand();
		is_selected = false;
		sprintf_s(name, MAX_NAME_LENGTH, "Object");
		flags = base_flags;
	}

	HierarchyNode(const char* Name, ImGuiTreeNodeFlags Flags)
	{
		id = lrand();
		is_selected = false;
		sprintf_s(name, MAX_NAME_LENGTH, "%s", Name);
		flags = Flags;
	}
	HierarchyNode(ImGuiTreeNodeFlags Flags)
	{
		id = lrand();
		is_selected = false;
		sprintf_s(name, MAX_NAME_LENGTH, "Object");
		flags = Flags;
	}
	~HierarchyNode() 
	{
		childs.clear();
	}

	void HierarchyNode::SetName(const char* Name) 
	{
		sprintf_s(name, MAX_NAME_LENGTH, "%s", Name);
	}

public:

	// Base flags
	static const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	static const ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

	// Selected flags
	static const ImGuiTreeNodeFlags base_selected_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
	static const ImGuiTreeNodeFlags leaf_selected_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Leaf;

private:

	long lrand()
	{
		if (sizeof(int) < sizeof(long))
			return (static_cast<long>(rand()) << (sizeof(int) * 8)) |
			rand();

		return rand();
	}
};


class Hierarchy
{

public:

	Hierarchy();
	~Hierarchy();

	static void AddNode(HierarchyNode* node = nullptr);

	//static Hierarchy hierarchy;
	static void Init();

	//Gui

	static void ShowHierarchy(bool* open);

	static void Draw(const char* title, bool* p_open);

	//Naming

	static bool SetSceneName(const char* name);

	static char scene_name[MAX_NAME_LENGTH];

	//Data

	static std::vector<HierarchyNode*> nodes;

	static std::set<HierarchyNode*> selected_nodes;


};

