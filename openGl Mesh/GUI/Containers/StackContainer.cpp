#include "StackContainer.h"

GUI::StackContainer::StackContainer() : IGUI_Container(), _spacing()
{
}

void GUI::StackContainer::render(Utils::Float2 origin, Utils::Float2 parentDimentions) const
{
	IGUI_Element::render(origin, parentDimentions);

	float offset = 0;
	Utils::Float2 drawPosition = getDrawPosition(origin, parentDimentions);
	Utils::Float2 drawDimentions = getDrawDimentions(parentDimentions);

	for (IGUI_Element* element : _children) {
		element->render(drawPosition + Utils::Float2(0, offset), drawDimentions);
		offset += _spacing.getPixelValue(drawDimentions.y);
		offset += element->getDrawDimentions(drawDimentions).y;
	}
}
