#include "GUI_Window.h"
#include "Utils/DrawData.h"
#include "IGUI_Container.h"

using namespace GUI;

Utils::Float2 GUI_Window::windowDimentions;

Utils::DrawData GUI_Window::getDrawData() const
{
	return _root->getDrawData();
}

void GUI_Window::setRoot(IGUI_Container* container)
{
	_root = container;
}
