#include "StackContainer.h"

void GUI::StackContainer::render(Utils::Float2 origin, Utils::Float2 parentDimentions) const
{
	IGUI_Element::render(origin, parentDimentions);

	Utils::Float2 offset(0, 0);
	Utils::Float2 drawPosition = getDrawPosition(origin, parentDimentions);
	Utils::Float2 drawDimentions = getDrawDimentions(parentDimentions);

	for (IGUI_Element* element : _children) {
		element->render(drawPosition + offset, drawDimentions);
		offset.y += element->getDrawDimentions(drawDimentions).y;
	}
}
