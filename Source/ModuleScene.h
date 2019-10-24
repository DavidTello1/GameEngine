#pragma once

#include "Module.h"
#include <vector>
#include "GameObject.h"


class ModuleScene :	public Module
{

public:

	ModuleScene(bool start_enabled = true);
	~ModuleScene();


	bool Start(Config* config = nullptr);
	void CreateShape(shape_type type, int slices, int stacks, float x = 0, float y = 0, float z = 0);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	bool Draw();

	// GameObjects-----------
public:
	GameObject* CreateGameObj(const char* name = "GameObject");
	void DeleteGameobj(GameObject* obj);
	GameObject* GetSelectedGameobj() { return selected_gameobj; }
	void SetSelectedGameobj(GameObject* obj) { selected_gameobj = obj; }

public:
	GameObject* selected_gameobj = nullptr;
	bool create_gameobj = false;

private:
	std::vector<GameObject*> gameobjs;
	// -----------------





};

