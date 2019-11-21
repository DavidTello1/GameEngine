#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Hierarchy.h"
#include "ModuleResources.h"
#include <vector>

class ComponentRenderer;
class ComponentMesh;

class ModuleScene :	public Module
{
public:

	ModuleScene(bool start_enabled = true);
	~ModuleScene();


	bool Start(Config* config = nullptr);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	bool Draw();

public:
	GameObject * FindById(uint id);
	GameObject* CreateGameObject(const char* name = "GameObject", GameObject* parent = nullptr, bool visible = false);
	void DeleteGameObject(GameObject* obj);
	void DeleteSelected();

	GameObject* GetSelectedGameObject() { 
		for (uint i=0;i<gameObjects.size();i++)
			if (gameObjects[i]->is_selected)
				return gameObjects[i];
		return nullptr;
	}

	void UnSelectAll(GameObject * keep_selected = nullptr);

	//void CreateShape(shape_type type, int slices, int stacks, float x = 0, float y = 0, float z = 0);

public:
	char scene_name[NAME_LENGTH];
	void SetSceneName(const char* name)	{strcpy_s(scene_name, name);}

	bool is_creating = false;
	bool is_selecting = false;

	std::vector<GameObject*> gameObjects;
	   
	static GameObject* root_object;

	GameObject* test_camera = nullptr;

private:

	GameObject* selected_gameobj = nullptr;

};