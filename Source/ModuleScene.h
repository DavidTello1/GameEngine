#pragma once

#include "Module.h"
#include <vector>
#include "Globals.h"
#include "GameObject.h"
#include "Hierarchy.h"
#include "ModuleResources.h"

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
	// GameObjects-----------
	GameObject* CreateGameObj(const char* name = "GameObject",const uint parent_id = 0, bool visible = false);
	void DeleteGameobj(GameObject* obj);
	GameObject* GetSelectedGameobj() { return selected_gameobj; }
	void SetSelectedGameobj(GameObject* obj) { selected_gameobj = obj; }

	bool IsMaterialLoaded(const char* path);
	void DeleteMaterial(ComponentMaterial* material);

public:
	bool create_gameobj = false;

	void EraseFromSelected(GameObject* go);
	std::vector<GameObject*> selected_go;
	std::vector<ComponentMaterial*> materials;
	std::vector<GameObject*> gameobjs;

private:
	GameObject* selected_gameobj = nullptr;
};

