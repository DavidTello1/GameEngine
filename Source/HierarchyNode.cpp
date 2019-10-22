#include "HierarchyNode.h"

HierarchyNode::HierarchyNode(HierarchyNode* Parent, GameObject* obj, ImGuiTreeNodeFlags Flags)
{
	is_selected = false;
	parent = Parent;
	flags = Flags;

	if (obj != nullptr)
	{
		id = obj->GetUID();
		name = obj->GetName();
	}
	else
	{
		id = 0;
		name = "unknown";
	}
}

HierarchyNode::~HierarchyNode()
{
}

bool HierarchyNode::ToggleSelection() // Toggles the state of the node, returns current state after toggled
{
	if (!is_selected) //not selected
	{
		flags |= ImGuiTreeNodeFlags_Selected;
		is_selected = true;

		LogAction("Selected");
	}
	else //selected
	{
		flags &= ~ImGuiTreeNodeFlags_Selected;
		is_selected = false;
		
		LogAction("Unselected");
	}

	return is_selected;
}

void HierarchyNode::SetName(const char* Name)
{
	LOG("Renaming node %d from '%s' to '%s'", id, name, Name);
	name = Name;
}

void HierarchyNode::LogAction(const char * Action)
{
	LOG("%s node '%s', id: %dd ", Action, name, id, 'd');
}
