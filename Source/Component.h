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
		Camera
	};

public:

	Component(Component::Type type, GameObject* object = nullptr);
	virtual ~Component() { Disable(); }

	virtual void Enable() { active = true; }
	virtual void Disable() { active = false; }

	bool IsActive() { return active; }
	void SwitchActive() { active = !active; }

	Component::Type GetType() { return type; }
	GameObject* GetGameobj() { return object; }

protected:
	bool active = true;
	GameObject* object = nullptr;
	Component::Type type;
};
