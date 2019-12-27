#pragma once
#include "UI_Element.h"

class Image : public UI_Element
{
public:
	Image(GameObject* gameObject, UI_Element::Type type);
	~Image();

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::IMAGE; }
};

