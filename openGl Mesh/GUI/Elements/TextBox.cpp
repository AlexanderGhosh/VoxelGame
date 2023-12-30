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

void GUI::TextBox::setPadding(Utils::Float2 padding, UNIT_MODE mode)
{
	_textPadding.set(padding, mode);
}

void GUI::TextBox::render(Utils::Float2 origin, Utils::Float2 parentDimentions) const
{
	IGUI_Element::render(origin, parentDimentions);
	Utils::Text::GlyphRendering& instance = Utils::Text::GlyphRendering::getInstance();

	Utils::Float2 drawPosition = getDrawPosition(origin, parentDimentions);
	Utils::Float2 drawDimentions = getDrawDimentions(parentDimentions);
	Utils::Float2 drawPadding = _textPadding.getPixelValue(drawDimentions);
	
	//instance.drawSentence(_text, (drawPosition + drawPadding), 0.5);
}
