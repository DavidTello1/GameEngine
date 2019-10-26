#include "ModuleSceneBase.h"
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
		name = "GameObject";
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
		
		App->scene->SetSelectedGameobj(obj);
		App->scene->selected_go.push_back(obj);
		//LogAction("Selected");
	}

	else // selected
	{
		flags &= ~ImGuiTreeNodeFlags_Selected;
		is_selected = false;
		
		App->scene->SetSelectedGameobj(nullptr);
		App->scene->EraseFromSelected(obj);
		//LogAction("Unselected");
	}

	return is_selected;
}

void HierarchyNode::SetName(const char* Name)
{
	LOG("Renaming node %d from '%s' to '%s'", id, name, Name,'v');
	name = Name;
}

void HierarchyNode::LogAction(const char * Action)
{
	LOG("%s node '%s', id: %ld ", Action, name, id, 'd');
}
