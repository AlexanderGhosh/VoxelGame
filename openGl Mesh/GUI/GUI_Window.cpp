#include "GUI_Window.h"
#include "Utils/DrawData.h"
#include "IGUI_Container.h"
#include <glad/glad.h>

using namespace GUI;

Utils::Float2 GUI_Window::windowDimentions;
unsigned int GUI_Window::elementShader;

void GUI::GUI_Window::render() const
{
	glEnable(GL_BLEND);
	_root->render({}, {1, 1});
}

void GUI_Window::setRoot(IGUI_Container* container)
{
	_root = container;
	_root->setPosition({0 , 0}, PIXELS);
	_root->setDimentions(windowDimentions, PIXELS);
}
