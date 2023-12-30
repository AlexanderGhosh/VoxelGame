#include "TextBox.h"
#include "../Utils/DrawData.h"
#include "../Utils/Text/GlyphRendering.h"
#include "../GUI_Window.h"

using namespace GUI;
using namespace Utils;

GUI::TextBox::TextBox() : IGUI_Element(), _text(""), _textPadding(PIXELS, 5, 5), _layoutType(LAYOUT_TYPE::CENTERED), _ensureMinmumSize(true), _textColour(0, 0, 0, 1)
{
}

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
	padding.y *= -1;
	_textPadding.set(padding, mode);
}

void GUI::TextBox::render(Utils::Float2 origin, Utils::Float2 parentDimentions) const
{
	IGUI_Element::render(origin, parentDimentions);
	Utils::Text::GlyphRendering& instance = Utils::Text::GlyphRendering::getInstance();

	Utils::Float2 textSize = instance.getSentenceLength(_text, 0.5);
	Utils::Float2 drawDimentions = getDrawDimentions(parentDimentions);
	if (_ensureMinmumSize) {
		bool dimsChanged = false;
		if (drawDimentions.x < textSize.x) {
			drawDimentions.x = textSize.x;
			dimsChanged = true;
		}
		if (drawDimentions.y < textSize.y) {
			drawDimentions.y = textSize.y;
			dimsChanged = true;
		}
		if(dimsChanged)
			const_cast<Utils::Units<Utils::Float2>&>(_dimentions).setPixel(drawDimentions);
	}

	Utils::Float2 drawPosition = getDrawPosition(origin, parentDimentions);
	Utils::Float2 drawPadding = _textPadding.getPixelValue(drawDimentions);


	Utils::Float2 layoutOffset; // added to drawPadding in order to satify the specified layout
	switch (_layoutType)
	{
	case GUI::TextBox::CENTERED:
		if (drawDimentions.x > textSize.x) {
			// is wide enough for no overlap in x dir
			layoutOffset.x = (drawDimentions.x - textSize.x) * .5f;
		}
		if (drawDimentions.y > textSize.y) {
			// is wide enough for no overlap in y dir
			layoutOffset.y = -(drawDimentions.y - textSize.y) * .5f;
		}
		break;
	case GUI::TextBox::VERTICAL:
		if (drawDimentions.y > textSize.y) {
			// is wide enough for no overlap in y dir
			layoutOffset.y = -(drawDimentions.y - textSize.y) * .5f;
		}
		break;
	case GUI::TextBox::HORIZONTAL:
		if (drawDimentions.x > textSize.x) {
			// is wide enough for no overlap in x dir
			layoutOffset.x = (drawDimentions.x - textSize.x) * .5f;
		}
		break;
	}

	
	drawPosition += layoutOffset + drawPadding; // adds padding and layout centering
	drawPosition.y = GUI_Window::windowDimentions.y - drawPosition.y; // flips axis
	drawPosition.y -= drawDimentions.y; // moves to default location bottom left of rect

	instance.drawSentence(_text, drawPosition, 0.5, _textColour);
}
