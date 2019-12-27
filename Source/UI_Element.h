#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>

struct Anchor
{
	float2 position;
	float4 corner_distance;
};

struct Pivot
{
	float2 position;
	float rotation;
};

class UI_Element : public Component
{
	friend class ModuleGUI;

public:
	enum Type
	{
		UNKNOWN = 0,
		CANVAS,
		IMAGE,
		TEXT,
		BUTTON,
		CHECKBOX,
		INPUTTEXT
	};

	enum State
	{
		NOTHING = 0,
		IDLE,
		HOVERED,
		SELECTED,
		LOCKED,
		DRAGGING
	};

	enum Action
	{
		NONE = 0
	};

public:
	UI_Element(UI_Element::Type type, GameObject* gameObject);
	virtual ~UI_Element();

	UI_Element::Type GetType() { return type; }
	UI_Element::State GetState() { return state; }
	UI_Element::Action GetAction() { return action; }

	void SwitchVisible() { visible = !visible; }
	void SwitchDraggable() { draggable = !draggable; }
	void SwitchInteractable() { interactable = !interactable; }
	void ChangeStateTo(State State) { state = State; }

	void UpdateState();
	void DoLogic(Action action);

	virtual void DrawInspector() = 0;

public:
	bool visible = false;
	bool draggable = false;
	bool interactable = false;

	float2 position;
	float2 size;

	Anchor anchor;
	Pivot pivot;
	float3 scale;

protected:
	Type type = UNKNOWN;
	State state = NOTHING;
	Action action = NONE;
};

