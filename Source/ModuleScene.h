#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Hierarchy.h"
#include "ModuleResources.h"
#include "Math.h" //AGDGSDGMSKGSDK
#include <vector>
#include "ImGuizmo.h"

class ComponentRenderer;
class ComponentMesh;
class ComponentCamera;
class Quadtree;
class Image;
class Text;
class Canvas;
class Button;
class CheckBox;
class InputText;

enum SceneState {
	EDIT,
	START,
	PLAY,
	PAUSE,
	STOP
};

class ModuleScene :	public Module
{
public:
	ALIGN_CLASS_TO_16

	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(Config * config);
	bool Start(Config* config = nullptr);
	bool Update(float dt);
	void UpdateTransformationGuizmos();
	bool PostUpdate(float dt);
	void RedoQuatree();
	bool CleanUp();

	bool Draw();

public:
	GameObject * FindById(uint id);
	GameObject* CreateGameObject(const char* name = "GameObject", GameObject* parent = nullptr, bool visible = false);
	void DeleteGameObject(GameObject* obj);
	void DeleteSelected();

	std::map<float, GameObject*> pick_candidates;
	GameObject* PickFromRay(Ray ray);

	GameObject* GetSelectedGameObject() { 
		for (uint i=0;i<gameObjects.size();i++)
			if (gameObjects[i]->is_selected)
				return gameObjects[i];
		return nullptr;
	}

	void UnSelectAll(GameObject * keep_selected = nullptr);

public:
	ImGuizmo::OPERATION current_guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE        curr_guizmo_mode = ImGuizmo::MODE::LOCAL;

	float3 last_scale = float3::zero;
	float4x4			  last_moved_transformation = float4x4::identity;

	std::string scene_name;
	void SetSceneName(const char* name) { scene_name = name; }

	bool is_creating = false;
	bool is_selecting = false;

	std::vector<GameObject*> gameObjects;
	   
	static GameObject* root_object;
	static SceneState state;
	static const char* state_to_string[STOP+1];
	ComponentCamera* test_camera = nullptr;
	GameObject* test_camera_obj = nullptr;

	Quadtree* quadtree;
private:
	GameObject* selected_gameobj = nullptr;


	//DEMO-----------------------
private:
	void MainMenu();
	void IngameWindow();

	Image* ingame_image = nullptr;
	CheckBox* ingame_checkbox = nullptr;
};