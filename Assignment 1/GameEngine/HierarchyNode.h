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
	std::vector<HierarchyNode*> childs;
	ImGuiTreeNodeFlags flags;

	HierarchyNode::HierarchyNode();
	HierarchyNode::HierarchyNode(const char* Name, ImGuiTreeNodeFlags Flags);
	HierarchyNode(ImGuiTreeNodeFlags Flags);
	~HierarchyNode();

	bool ToggleSelection();

	void SetName(const char * Name);
	

public:

	// Base flags
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
