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

	void Draw(ComponentCamera* camera);

public:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;

	glfreetype::font_data font;
	float font_size = DEFAULT_FONT_SIZE;
	char text[MAX_TEXT_SIZE]   = "Text";
	char buffer[MAX_TEXT_SIZE] = "Text";

	Color text_color = Color::black;
	float2 text_pos = float2::zero;

private:
	const char* action_list[1] = {
	"NONE"
	};

};
