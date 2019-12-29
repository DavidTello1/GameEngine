#pragma once
#include "UI_Element.h"
#include "Canvas.h"
#include "ResourceMaterial.h"

class Image : public UI_Element
{
	friend class Component;

public:
	Image(GameObject* gameObject, UI_Element::Type type);
	~Image();

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::IMAGE; }

	void Draw(ComponentCamera* camera);

public:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;
};

