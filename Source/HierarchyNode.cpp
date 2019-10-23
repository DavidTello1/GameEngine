#include "ModuleSceneIntro.h"
#include "Application.h"
#include "HierarchyNode.h"

HierarchyNode::HierarchyNode(GameObject* Obj, HierarchyNode* Parent, ImGuiTreeNodeFlags Flags) {
	is_selected = false;
	parent = Parent;
	flags = Flags;

	obj = Obj;
	if (obj != nullptr)
	{
		id = obj->GetUID();
		name = obj->GetName();
	}
	else
	{
		id = 0;
		name = "Unknown";
	}
}

HierarchyNode::~HierarchyNode()
{
}

bool HierarchyNode::ToggleSelection() // Toggles the state of the node, returns current state after toggled
{
	if (!is_selected) // not selected
	{
		flags |= ImGuiTreeNodeFlags_Selected;
		is_selected = true;
		
		App->scene_intro->SetSelectedGameobj(obj);
		//LogAction("Selected");
	}

	else // selected
	{
		flags &= ~ImGuiTreeNodeFlags_Selected;
		is_selected = false;
		
		App->scene_intro->SetSelectedGameobj(nullptr);
		//LogAction("Unselected");
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
	LOG("%s node '%s', id: %ld ", Action, name, id, 'd');
}
