#include "UI_Element.h"
#include "Application.h"

UI_Element::UI_Element(UI_Element::Type Type, GameObject* gameObject) : Component(Component::Type::UI_Element, gameObject), type(Type)
{
}

UI_Element::~UI_Element()
{
}

void UI_Element::UpdateCollider()
{
	collider.x =  position2D.x;
	collider.y = position2D.y;
	collider.w = size2D.x * scale2D.x;
	collider.h = size2D.y * scale2D.y;
}

bool UI_Element::CheckMousePos()
{
	App->input->GetMousePosition(mouse_pos);
	SDL_Rect MouseCollider = { mouse_pos.x,mouse_pos.y,1,1 };

	if (SDL_HasIntersection(&MouseCollider, &collider))
		return true;

	return false;
}

bool UI_Element::CheckClick()
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->input->GetMousePosition(drag_start);
		return true;
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		return true;

	return false;
}

void UI_Element::UpdateState()
{
	if (interactable)
	{
		if (state != DRAGGING)
		{
			if (CheckMousePos())
			{
				ChangeStateTo(HOVERED);
				if (CheckClick())
				{
					if (draggable && (drag_start.x != mouse_pos.x || drag_start.y != mouse_pos.y))
						ChangeStateTo(DRAGGING);
					else
						ChangeStateTo(SELECTED);
				}
			}
			else
				ChangeStateTo(IDLE);
		}
		else
		{
			if (!CheckClick())
			{
				ChangeStateTo(IDLE);
			}
		}
	}
}

void UI_Element::DoLogic(Action action)
{
	switch (action)
	{
	case NONE:
		break;
	}
}