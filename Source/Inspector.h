#pragma once

#include "Panel.h"
#include "Math.h"

class Inspector : public Panel
{
public:
	static const uint default_width = 302;
	static const uint default_height = 862;
	static const uint default_pos_x = 978;
	static const uint default_pos_y = 19;

public:
	Inspector();
	~Inspector();

	void Draw();
//	void SetTranslation(float x, float y, float z);
//	void SetRotation(float x, float y, float z);
//	void SetScale(float x, float y, float z);
//
//	//void GetTriangles();
//
//private:
//	float3 translation, rotation, scale;
	float min, max;
};