#ifndef __ASSETS_H__
#define __ASSETS_H__

#include "Panel.h"
#include "PathNode.h"
#include "Timer.h"
#include "Imgui/imgui.h"
#include <vector>

#define ICON_WIDTH 64
#define ICON_HEIGHT 64

class Assets : public Panel
{
public:
	static const uint default_width = 1280;
	static const uint default_height = 323;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 701;

public:
	Assets();
	virtual ~Assets();

	void Draw();
	void ImportAsset();

private:
	void UpdateAssets();
	void DrawHierarchy(const PathNode& node);
	void DrawIcons(const PathNode& node);
	uint GetIcon(const PathNode& node);

public:
	uint folder_icon;
	uint file_icon;
	uint model_icon;
	uint material_icon;
	uint scene_icon;

private:
	std::vector<uint> selected_assets;

	PathNode assets;
	PathNode current_node;
	PathNode next_node;

	PathNode selected_node;

	Timer timer;
	uint rate = 5;

	ImVec4 border_color;
};

#endif// __ASSETS_H__
