#pragma once
#include "UI_Element.h"
#include "ResourceMaterial.h"
#include <vector>

class Canvas : public UI_Element
{
public:
	Canvas(GameObject* gameObject, UI_Element::Type type);
	~Canvas();

	void AddElement(UI_Element * element);

	void Draw(ComponentCamera * camera);

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::CANVAS; }

private:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;

	std::vector<UI_Element*> elements;
};

