#ifndef __HIERARCHY_H__
#define __HIERARCHY_H__

#include "Panel.h"
#include "Imgui/imgui.h"
#include "Math.h"
#include <random>
#include <vector>

class GameObject;

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

	char buffer[NAME_LENGTH];

private:
	void DrawCreateMenu();

private:
	bool rename = false;

};

#endif// __HIERARCHY_H__
