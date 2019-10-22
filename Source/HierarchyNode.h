#pragma once
#include "Globals.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include <vector>

class HierarchyNode
{
public:
	HierarchyNode::HierarchyNode(GameObject* object = nullptr, HierarchyNode* Parent = nullptr, ImGuiTreeNodeFlags Flags = leaf_flags);	~HierarchyNode();

	bool ToggleSelection();
	void SetName(const char * Name);
	void LogAction(const char* Action);

public:

	// flags
	static const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	static const ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

	bool is_selected;
	const char* name;
	uint id;
	HierarchyNode* parent;
	std::vector<HierarchyNode*> childs;
	ImGuiTreeNodeFlags flags;

	GameObject* obj;
};

