#include "GUI_Window.h"
#include "Utils/DrawData.h"
#include "IGUI_Container.h"

using namespace GUI;

Utils::DrawData GUI::GUI_Window::getDrawData() const
{
	return _root->getDrawData();
}

void GUI::GUI_Window::setRoot(IGUI_Container* container)
{
	_root = container;
}
