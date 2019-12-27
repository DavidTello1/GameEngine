#pragma once
#include "UI_Element.h"

class Image : public UI_Element
{
public:
	Image(GameObject* gameObject, UI_Element::Type type);
	~Image();
};

