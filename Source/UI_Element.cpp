#include "UI_Element.h"

UI_Element::UI_Element(UI_Element::Type Type, GameObject* gameObject) : Component(Component::Type::UI_Element, gameObject), type(Type)
{
}

UI_Element::~UI_Element()
{
}
