#include "TextBox.h"
#include "../Utils/DrawData.h"
#include "../Utils/Text/GlyphRendering.h"
#include "../GUI_Window.h"

using namespace GUI;
using namespace Utils;

DrawData TextBox::getDrawData() const
{
	return IGUI_Element::getDrawData();
}

void GUI::TextBox::setText(const std::string& text)
{
	_text = text;
}

void GUI::TextBox::setPadding(Utils::Float2 padding, SIZE_MODE mode)
{
	_textPadding = padding;
	if (mode == SIZE_MODE::PIXELS)
		_textPadding /= GUI_Window::windowDimentions;
}

void GUI::TextBox::render() const
{
	IGUI_Element::render();
	Utils::Text::GlyphRendering& instance = Utils::Text::GlyphRendering::getInstance();
	
	instance.drawSentence(_text, (_position + _textPadding) * GUI_Window::windowDimentions, 0.5);
}
