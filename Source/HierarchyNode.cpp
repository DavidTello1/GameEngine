#include "ModuleSceneBase.h"
#include "Application.h"
#include "HierarchyNode.h"

HierarchyNode::HierarchyNode(GameObject* gameObject, GameObject* Parent ){

	is_selected = false;
	is_rename = false;
	flags = leaf_flags;
	object = gameObject;
	parent = Parent;

	if (parent != nullptr)
	{
		parent->node->flags &= ~ImGuiTreeNodeFlags_Leaf;
	}

}

HierarchyNode::~HierarchyNode()
{
	if (parent != nullptr && !parent->HasChilds())
	{
		parent->node->flags |= ImGuiTreeNodeFlags_Leaf;
	}
}

bool HierarchyNode::Select()
{
	flags |= ImGuiTreeNodeFlags_Selected;
	is_selected = true;
}

bool HierarchyNode::UnSelect()
{
	flags &= ~ImGuiTreeNodeFlags_Selected;
	is_selected = false;
}

bool HierarchyNode::ToggleSelection() // Toggles the state of the node, returns current state after toggled
{
	if (!is_selected) // not selected
	{
		Select();
	}
	else // selected
	{
		UnSelect();
	}

	return is_selected;
}

void HierarchyNode::LogAction(const char * Action)
{
	LOG("%s node '%s', id: %ld ", Action, object->GetName(), object->GetUID(), 'd');
}
