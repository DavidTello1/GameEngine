#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>

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

	//void ChangePosition();
	//void ChangeAnchorPos();
	//void ChangeRotation();
	//void ChangePivotPos();
	//void ChangeScale();
	//void ChangeSize();

	void ChangeStateTo(State new_state) { state = new_state; }
	void UpdateState();
	void DoLogic(Action action);

	void DrawInspector() {};

public:
	bool visible = true;
	bool draggable = true;
	bool interactable = true;

	float2 position2D = float2::zero;
	float2 anchor = float2::zero;

	float rotation2D = 0.0f;
	float2 pivot = float2::zero;
	
	float2 scale2D = float2::zero;
	float2 size2D = float2::zero;


protected:
	Type type = UNKNOWN;
	State state = NOTHING;
	Action action = NONE;
};

