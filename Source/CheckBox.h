#pragma once
#include "UI_Element.h"
#include "Canvas.h"
#include "ResourceMaterial.h"
#include "Text.h"

class CheckBox : public UI_Element
{
	friend class Component;

public:
	CheckBox(GameObject* gameObject, UI_Element::Type type);
	~CheckBox();

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::CHECKBOX; }

	void Draw(ComponentCamera* camera);

public:
	Canvas* canvas = nullptr;
	ResourceMaterial* unselected = nullptr;
	ResourceMaterial* selected = nullptr;

	glfreetype::font_data font;
	float font_size = 20;
	char text[MAX_TEXT_SIZE]   = "CheckBox";
	char buffer[MAX_TEXT_SIZE] = "CheckBox";

	Color text_color = Color::black;
	float2 text_pos = { 30,-13 };

private:
	Color idle_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color hovered_color = { 0.5f, 0.5f, 0.5f, 1.0f };
	Color selected_color = { 0.5f, 0.0f, 0.0f, 1.0f };
	Color locked_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	bool is_selected = false;
};
