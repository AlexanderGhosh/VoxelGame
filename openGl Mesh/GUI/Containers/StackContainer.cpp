#include "StackContainer.h"

void GUI::StackContainer::render(Utils::Float2 origin, Utils::Float2 parentDimentions) const
{
	IGUI_Container::render(origin, parentDimentions);
	Utils::Float2 offset(0, 0);
	for (IGUI_Element* element : _children) {
		// element->render(_position + origin + offset, _dimentions);
		// offset.y += element->getDimentions().y;
	}
}
