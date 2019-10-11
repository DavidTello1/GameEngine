#pragma once

#include "Globals.h"
#include <string>

class Config;

class Resources
{
public:
	enum Type
	{
		model = 0,
		texture,
		mesh,
		unknown
	};

public:
	Resources(Resources::Type type);
	virtual ~Resources();
};