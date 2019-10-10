#pragma once
#include "Panel.h"
class Inspector : public Panel
{
public:

	Inspector();
	~Inspector();

	static const uint default_width = 302;
	static const uint default_height = 862;
	static const uint default_pos_x = 978;
	static const uint default_pos_y = 19;


	void Draw();
};

