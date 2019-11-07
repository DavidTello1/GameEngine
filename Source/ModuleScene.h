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
	void UpdateMainCamera(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	bool Draw();

public:
	GameObject * FindById(uint id);
	GameObject* CreateGameObject(const char* name = "GameObject", GameObject* parent = nullptr, bool visible = false);
	void DeleteGameObject(GameObject* obj);
	void DeleteSelected();

	GameObject* GetSelectedGameObject() { 
		for (int i=0;i<gameObjects.size();i++)
			if (gameObjects[i]->is_selected)
				return gameObjects[i];
		return nullptr;
	}

	void UnSelectAll(GameObject * keep_selected = nullptr);

	bool IsMaterialLoaded(const char* path);
	ComponentMaterial* GetMaterial(const char* path) const;
	bool DeleteMaterial(ComponentMaterial* material);

	//void CreateShape(shape_type type, int slices, int stacks, float x = 0, float y = 0, float z = 0);

public:
	char scene_name[NAME_LENGTH];
	void SetSceneName(const char* name)	{strcpy_s(scene_name, name);}

	bool create_gameobj = false;

	std::vector<ComponentMaterial*> materials;
	std::vector<GameObject*> gameObjects;


	// Misc
	float wireframe_color[3] = { 1.0f, 1.0f, 1.0f };
	float wireframe_width = 1.0f;
	bool show_all_wireframe = false;

	float bounding_box_color[3] = { 1.0f, 0.0f, 0.0f };
	float bounding_box_width = 1.0f;
	bool show_all_bounding_box = false;
	   
	static GameObject* root_object;
	static GameObject* main_camera;

	float zoom_speed = 75.0f;

private:
	GameObject* selected_gameobj = nullptr;

	//Hierarchy --------------------------------

public:
};