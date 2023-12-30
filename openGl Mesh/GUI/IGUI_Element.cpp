#include "IGUI_Element.h"
#include <glad/glad.h>
#include "GUI_Window.h"

using namespace GUI;
static void addQuad(Utils::DrawData& data, const Utils::Float2& offset, const Utils::Float2& dims);


IGUI_Element::IGUI_Element() : _position(), _dimentions(), _backgroundColour(), _cornerRadius(), _boarderColour(), _drawBuffer(), _changed(false), _boarderSize() {
	_drawBuffer.setUp(nullptr, 0);
}

Utils::DrawData IGUI_Element::getDrawData() const {
	Utils::DrawData d;
	// center quad
	
	Utils::Float2 offset(0, 0);
	Utils::Float2 dims(1, 1);

	if(_backgroundColour.a > 0)
		addQuad(d, offset, dims);
	return d;
}

void IGUI_Element::render(Utils::Float2 origin, Utils::Float2 parentDimentions) const
{
	if (_changed) {
		const Utils::DrawData data = getDrawData();
		_drawBuffer.realoc(data.data(), data.size());
		_changed = false;
	}
	if (_drawBuffer.size() == 0) return;

	Utils::Float2 drawPosition = getDrawPosition(origin, parentDimentions);
	Utils::Float2 drawDimentions = getDrawDimentions(parentDimentions);

	glUseProgram(GUI_Window::elementShader);

	unsigned int loc = glGetUniformLocation(GUI_Window::elementShader, "position");
	glUniform2f(loc, drawPosition.x, drawPosition.y);

	loc = glGetUniformLocation(GUI_Window::elementShader, "dimentions");
	glUniform2f(loc, drawDimentions.x, drawDimentions.y);

	loc = glGetUniformLocation(GUI_Window::elementShader, "cornerRadius");
	glUniform1f(loc, _cornerRadius.getPixelValue(drawDimentions.magnitude()));

	loc = glGetUniformLocation(GUI_Window::elementShader, "borderSize");
	glUniform1f(loc, _boarderSize.getPixelValue(drawDimentions.magnitude()));

	loc = glGetUniformLocation(GUI_Window::elementShader, "bgColour");
	glUniform4f(loc, _backgroundColour.r, _backgroundColour.g, _backgroundColour.b, _backgroundColour.a);

	loc = glGetUniformLocation(GUI_Window::elementShader, "borderColour");
	glUniform4f(loc, _boarderColour.r, _boarderColour.g, _boarderColour.b, _boarderColour.a);

	_drawBuffer.bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, _drawBuffer.size());
	_drawBuffer.unBind();
}

void IGUI_Element::setPosition(Utils::Float2 pos, UNIT_MODE mode)
{
	_position.set(pos, mode);
	_changed = true;
}

void IGUI_Element::setDimentions(Utils::Float2 dims, UNIT_MODE mode)
{
	_dimentions.set(dims, mode);
	_changed = true;
}

void IGUI_Element::setBorderSize(float size, UNIT_MODE mode) {
	_boarderSize.set(size, mode);
	_changed = true;
}

void IGUI_Element::setCornerRadius(float radius, UNIT_MODE mode)
{
	_cornerRadius.set(radius, mode);
	_changed = true;
}

static void addQuad(Utils::DrawData& data, const Utils::Float2& offset, const Utils::Float2& dims) {
	data.push_back(Utils::Vertex({ offset.x, offset.y }));
	data.push_back(Utils::Vertex({ offset.x + dims.x, offset.y }));
	data.push_back(Utils::Vertex({ offset.x, offset.y + dims.y }));
	data.push_back(Utils::Vertex({ offset.x + dims.x, offset.y + dims.y }));
}