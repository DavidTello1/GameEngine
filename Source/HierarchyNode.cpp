#include "HierarchyNode.h"

HierarchyNode::HierarchyNode()
{
	id = lrand();
	is_selected = false;
	parent = nullptr;
	sprintf_s(name, MAX_NAME_LENGTH, "Object");
	flags = base_flags;
}

HierarchyNode::HierarchyNode(HierarchyNode* Parent, const char* Name, ImGuiTreeNodeFlags Flags)
{
	id = lrand();
	is_selected = false;
	parent = Parent;
	sprintf_s(name, MAX_NAME_LENGTH, "%s", Name);
	flags = Flags;
}

HierarchyNode::~HierarchyNode(){}


// Toggles the state of the node, returns current state after toggled
bool HierarchyNode::ToggleSelection()
{
	// if its NOT selected
	if (!is_selected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
		is_selected = true;

		LogAction("Selected");
	}

	// if its selected
	else {
		flags &= ~ImGuiTreeNodeFlags_Selected;
		is_selected = false;
		
		LogAction("Unselected");
	}

	return is_selected;
}

void HierarchyNode::SetName(const char* Name)
{
	if (strlen(Name) < MAX_NAME_LENGTH) {
		LOG("Renaming node %ld from '%s' to '%s'", id, name, Name);
		sprintf_s(name, MAX_NAME_LENGTH, "%s", Name);
	}
	else {
		LOG("Node name exceeds max length (%d)",MAX_NAME_LENGTH ,'e');
	}
}

void HierarchyNode::LogAction(const char * Action)
{
	LOG("%s node '%s', id: %ld ", Action, name, id, 'd');
}
