#ifndef __HIERARCHY_H__
#define __HIERARCHY_H__

#include "Panel.h"
#include "Imgui/imgui.h"
#include "Math.h"
#include <random>
#include <vector>

class Hierarchy : public Panel
{
public:
	static const uint default_width = 262;
	static const uint default_height = 862;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 19;

public:
	Hierarchy();
	virtual ~Hierarchy();

	void Draw();
	void DrawNodes(std::vector<GameObject*>& v);

	char buffer[120];

	//static void AddNode(GameObject* object, GameObject* parent = root_obj);
	//static void DeleteNode(GameObject * n);
	//static void DeleteSelected();
	//static bool SearchAndDeleteNode(GameObject* n, std::vector<GameObject*>& v);

	//void UnSelectAll(GameObject* keep_selected = nullptr);

	//void SetSceneName(const char* name);

	//static void GetMinMaxVertex(GameObject* node, float3 * abs_min, float3 * abs_max);

private:
	//void DrawCreateMenu();

public:
	//Data
	//static GameObject* root_obj;

	// JUST A REFERENCE TO THE SCENE GAMEOBJECTS
	//std::vector<GameObject*>* nodes;

	//char scene_name[NAME_LENGTH];
};

#endif// __HIERARCHY_H__
