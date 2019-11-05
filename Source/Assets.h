#ifndef __ASSETS_H__
#define __ASSETS_H__

#include "Panel.h"
#include "Imgui/imgui.h"
#include <vector>

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
	std::vector<uint> selected_assets;
};

#endif// __ASSETS_H__
