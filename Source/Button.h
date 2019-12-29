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
	static inline UI_Element::Type GetType() { return UI_Element::Type::BUTTON; }

	void Draw(ComponentCamera* camera);

public:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;

	glfreetype::font_data font;
	float font_size = 20;
	char text[MAX_TEXT_SIZE]   = "Button";
	char buffer[MAX_TEXT_SIZE] = "Button";

	Color text_color = Color::black;
	float2 text_pos = { -35,-11 };

private:
	Color idle_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color hovered_color = { 0.5f, 0.5f, 0.5f, 1.0f };
	Color selected_color = { 0.25f, 0.25f, 1.0f, 1.0f };
	Color locked_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};
