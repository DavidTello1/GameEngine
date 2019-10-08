#pragma once
#include <string>

class Application;
struct PhysBody3D;

#define MODULE_NAME_LENGTH 25
class Module
{
private:
	bool enabled;
	char name[MODULE_NAME_LENGTH];

public:
	Module(const char* name, bool start_enabled = true) : enabled(start_enabled)
	{
		strcpy_s(this->name, MODULE_NAME_LENGTH, name);
	}

	virtual ~Module()
	{}

	const char* GetName() const
	{
		return name;
	}

	bool IsActive() const
	{
		return enabled;
	}

	void SetActive(bool active)
	{
		if (enabled != active)
		{
			enabled = active;
			if (active == true)
				Start();
			else
				CleanUp();
		}
	}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool PreUpdate(float dt)
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool PostUpdate(float dt)
	{
		return true;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}
};