#pragma once
#include "UI_Element.h"
#include "Canvas.h"
#include "ResourceMaterial.h"
#include "Text.h"

class InputText : public UI_Element
{
	friend class Component;

public:
	InputText(GameObject* gameObject, UI_Element::Type type);
	~InputText();

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::INPUTTEXT; }

	void Draw(ComponentCamera* camera);

public:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;

	glfreetype::font_data font;
	float font_size = 20;
	char text[MAX_TEXT_SIZE]   = "InputText";
	char buffer[MAX_TEXT_SIZE] = "InputText";

	Color text_color = Color::black;
	float2 text_pos = { -47,-13 };

private:
	Color idle_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color hovered_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color selected_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	bool is_selected = false;
};
