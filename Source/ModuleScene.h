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
	GameObject * FindById(uint id);
	// GameObjects-----------
	GameObject* CreateGameObj(const char* name = "GameObject",const uint parent_id = 0, bool visible = false);
	void DeleteGameobj(GameObject* obj);
	GameObject* GetSelectedGameobj() { return selected_gameobj; }
	void SetSelectedGameobj(GameObject* obj) { selected_gameobj = obj; }

	bool IsMaterialLoaded(const char* path);
	ComponentMaterial* GetMaterial(const char* path) const;
	bool DeleteMaterial(ComponentMaterial* material);

	//void CreateShape(shape_type type, int slices, int stacks, float x = 0, float y = 0, float z = 0);

public:
	bool create_gameobj = false;

	void EraseFromSelected(GameObject* go);
	std::vector<GameObject*> selected_go;
	std::vector<ComponentMaterial*> materials;
	std::vector<GameObject*> gameobjs;


	// Misc
	float wireframe_color[3] = { 1.0f, 1.0f, 1.0f };
	float wireframe_width = 1.0f;
	bool show_all_wireframe = false;

	float bounding_box_color[3] = { 1.0f, 0.0f, 0.0f };
	float bounding_box_width = 1.0f;
	bool show_all_bounding_box = false;
	   
private:
	GameObject* selected_gameobj = nullptr;
};