#include "HierarchyNode.h"
#include "Hierarchy.h"


HierarchyNode::HierarchyNode()
{
	id = lrand();
	is_selected = false;
	sprintf_s(name, MAX_NAME_LENGTH, "Object");
	flags = base_flags;
}
HierarchyNode::HierarchyNode(ImGuiTreeNodeFlags Flags)
{
	id = lrand();
	is_selected = false;
	sprintf_s(name, MAX_NAME_LENGTH, "Object");
	flags = Flags;
}

HierarchyNode::HierarchyNode(const char* Name, ImGuiTreeNodeFlags Flags)
{
	id = lrand();
	is_selected = false;
	sprintf_s(name, MAX_NAME_LENGTH, "%s", Name);
	flags = Flags;
}

HierarchyNode::~HierarchyNode()
{
	/*for (HierarchyNode* child : childs)
	{
		delete(child);
		
	}
	childs.clear();*/
}


// Toggles the state of the node, returns current state after toggled
bool HierarchyNode::ToggleSelection()
{
	// if its NOT selected
	if (!is_selected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
		is_selected = true;

		LOG("NODE %ld SELECTED", id, 'd');
	}

	// if its selected
	else {
		flags &= ~ImGuiTreeNodeFlags_Selected;
		is_selected = false;
		
		LOG("NODE %ld UN-SELECTED", id, 'd');
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