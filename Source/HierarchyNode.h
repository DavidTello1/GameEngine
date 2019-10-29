#pragma once
#include "Globals.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include <vector>

class HierarchyNode
{
public:
	HierarchyNode(GameObject * gameObject = nullptr, GameObject * Parent = nullptr);
	~HierarchyNode();

	bool Select();
	bool UnSelect();
	bool ToggleSelection();

	// Debug
	void LogAction(const char* Action);

public:
	uint position = 0;

	bool is_selected = false;
	bool is_rename = false;

	// Objects
	GameObject* parent;
	GameObject* object;
	std::vector<GameObject*> childs;

	// flags
	ImGuiTreeNodeFlags flags;

	static const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	static const ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
};

