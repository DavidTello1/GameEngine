#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>
#include "SDL/include/SDL_rect.h"
#include "Color.h"

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

	void ChangeSize(float2 size) { size2D = size; }
	void ChangePosition(float2 pos) { position2D = pos; }
	void ChangeRotation(float rot) { rotation2D = rot; }
	void ChangeScale(float2 scale) { scale2D = scale; }

	void UpdateCollider();
	void ChangeStateTo(State new_state) { state = new_state; }
	void UpdateState();
	void DoLogic(Action action);

	virtual void Draw() {};
	void DrawInspector() {};

private:
	bool CheckMousePos();
	bool CheckClick();

public:
	bool visible = true;
	bool draggable = true;
	bool interactable = true;

	float2 size2D = float2::zero;
	float2 position2D = float2::zero;
	float rotation2D = 0.0f;
	float2 scale2D = float2::zero;

	Color color = Color::white;

protected:
	Type type = UNKNOWN;
	State state = NOTHING;
	Action action = NONE;

private:
	float2 drag_start = float2::zero;
	float2 mouse_pos = float2::zero;

	SDL_Rect collider;
};

