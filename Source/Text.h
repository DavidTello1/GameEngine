#pragma once
#include "UI_Element.h"
#include "Canvas.h"
#include "ResourceMaterial.h"
#include "TextRenderer.hpp"

#define DEFAULT_FONT_SIZE 25
#define MAX_TEXT_SIZE 256

class Text : public UI_Element
{
public:
	Text(GameObject* gameObject, UI_Element::Type type);
	~Text();

	void LoadFont(const char * path, int size);

	void DrawInspector() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::TEXT; }

	void Draw(ComponentCamera* camera) override;

public:
	Canvas* canvas = nullptr;
	ResourceMaterial* material = nullptr;
	glfreetype::font_data font;

	float font_size = DEFAULT_FONT_SIZE;

	char text[MAX_TEXT_SIZE]   = "Text";
	char buffer[MAX_TEXT_SIZE] = "Text";
};

