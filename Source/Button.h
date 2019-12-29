#pragma once
#include "UI_Element.h"
#include "Canvas.h"
#include "ResourceMaterial.h"
#include "Text.h"

class Button : public UI_Element
{
	friend class Component;

public:
	Button(GameObject* gameObject, UI_Element::Type type);
	~Button();

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::IMAGE; }

public:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;
	Text* text = nullptr;

private:
	const char* action_list[1] = {
	"NONE"
	};

};
