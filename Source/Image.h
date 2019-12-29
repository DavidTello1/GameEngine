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

	void Draw(ComponentCamera* camera);

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::IMAGE; }

private:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;
};

