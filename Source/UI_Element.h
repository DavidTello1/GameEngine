#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>
#include "SDL/include/SDL_rect.h"
#include "Color.h"
#include "ComponentCamera.h"

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
		NONE = 0,
		SWITCH_VSYNC,

	};

public:
	UI_Element(UI_Element::Type type, GameObject* gameObject);
	virtual ~UI_Element();

	UI_Element::Type GetType() { return type; }
	UI_Element::State GetState() { return state; }
	UI_Element::Action GetAction() { return action; }

	void ChangeActionTo(Action new_action) { action = new_action; }
	void ChangeStateTo(State new_state) { state = new_state; }
	void ChangeColor(Color new_color) { color = new_color; }
	void ChangeSize(float2 size) { size2D = size; }
	void ChangePosition(float2 pos) { position2D = pos; }
	void ChangeRotation(float rot) { rotation2D = rot; }
	void ChangeScale(float2 scale) { scale2D = scale; }

	void UpdateCollider();
	void UpdateState();
	void DoLogic(Action action);

	virtual void Draw(ComponentCamera* camera) {};
	virtual void DrawInspector() {};

private:
	bool CheckMousePos();
	bool CheckClick();

public:
	bool visible = true;
	bool draggable = true;
	bool interactable = true;

	float2 size2D = { 50,50 };
	float2 position2D = { 50,50 };
	float rotation2D = 0.0f;
	float2 scale2D = { 1,1 };

	Color color = Color::white;

protected:
	Type type = UNKNOWN;
	State state = NOTHING;
	Action action = NONE;

	float2 drag_start = float2::zero;
	float2 mouse_pos = float2::zero;

	SDL_Rect collider;

	const char* action_list[2] = {
	"None",
	"Switch VSync"
	};

};

