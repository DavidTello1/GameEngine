#pragma once
#include "Globals.h"
#include <vector>
#include "ImGui/imgui.h"
#include <set>

#define MAX_NAME_LENGTH 512

class HierarchyNode
{

public:
	long id;
	bool is_selected;
	char name[MAX_NAME_LENGTH];
	HierarchyNode* parent;
	std::set<HierarchyNode*> childs;
	ImGuiTreeNodeFlags flags;

	HierarchyNode::HierarchyNode();
	HierarchyNode(ImGuiTreeNodeFlags Flags);
	HierarchyNode(ImGuiTreeNodeFlags Flags, HierarchyNode* Parent);
	HierarchyNode::HierarchyNode(const char* Name, ImGuiTreeNodeFlags Flags);
	~HierarchyNode();

	bool ToggleSelection();

	void SetName(const char * Name);
	

public:

	// flags
	static const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	static const ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

private:

	long lrand()
	{
		if (sizeof(int) < sizeof(long))
			return (static_cast<long>(rand()) << (sizeof(int) * 8)) |
			rand();

		return rand();
	}
};

