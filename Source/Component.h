#pragma once
#include "Globals.h"

class GameObject;

class Component
{
public:
	enum class Type
	{
		Unknown,
		Mesh,
		Renderer,
		Material,
		Camera,
		UI_Element
	};

public:
	Component(Component::Type type, GameObject* object = nullptr);
	virtual ~Component() { Disable(); }

	virtual void Enable() { active = true; }
	virtual void Disable() { active = false; }

	bool IsActive() { return active; }
	void SwitchActive() { active = !active; }

	GameObject* GetGameobj() { return object; }

	virtual void DrawInspector() = 0;
	mutable Component::Type type;

protected:
	GameObject* object = nullptr;
	bool active = true;
};
