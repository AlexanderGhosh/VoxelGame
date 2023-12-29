#include "IGUI_Element.h"
#include <glad/glad.h>
#include "GUI_Window.h"

using namespace GUI;
static void addQuad(Utils::DrawData& data, const Utils::Float2& offset, const Utils::Float2& dims, const Utils::Float4& colour);


IGUI_Element::IGUI_Element() : _position(), _dimentions(), _backgroundColour(), _cornerRadius(), _boarderColour(), _drawBuffer(), _changed(false) {
	_drawBuffer.setUp(nullptr, 0);
}

Utils::DrawData IGUI_Element::getDrawData() const {
	Utils::DrawData d;
	// center quad
	
	Utils::Float2 offset = _position + _cornerRadius;
	Utils::Float2 dims = _dimentions - _cornerRadius * 2.;

	if(_backgroundColour.a > 0)
		addQuad(d, offset, dims, _backgroundColour);

	if (_cornerRadius.x > 0 && _cornerRadius.y && _boarderColour.a > 0) {
		// top quad
		offset = _position + Utils::Float2(_cornerRadius.x, _dimentions.y - _cornerRadius.y);
		dims = Utils::Float2(_dimentions.x - 2 * _cornerRadius.x, _cornerRadius.y);
		addQuad(d, offset, dims, _boarderColour);

		// bottom quad
		offset = _position + Utils::Float2(_cornerRadius.x, 0);
		dims = Utils::Float2(_dimentions.x - 2 * _cornerRadius.x, _cornerRadius.y);
		addQuad(d, offset, dims, _boarderColour);

		// left quad
		offset = _position + Utils::Float2(0, _cornerRadius.y);
		dims = Utils::Float2(_cornerRadius.x, _dimentions.y - 2 * _cornerRadius.y);
		addQuad(d, offset, dims, _boarderColour);

		// right quad
		offset = _position + Utils::Float2(_dimentions.x - _cornerRadius.x, _cornerRadius.y);
		dims = Utils::Float2(_cornerRadius.x, _dimentions.y - 2 * _cornerRadius.y);
		addQuad(d, offset, dims, _boarderColour);

		// all corners
		float num_points = 6;
		float num_points_inc = (0.5f * 3.14f) / num_points;

		Utils::Float2 base(1, 0);
		const Utils::Float2 p10 = _position + _dimentions - _cornerRadius; // top right
		const Utils::Float2 p11(_position.x + _cornerRadius.x, p10.y);                     // top left
		const Utils::Float2 p12(p10.x, _position.y + _cornerRadius.y);                     // bottom right
		const Utils::Float2 p13(_position.x + _cornerRadius.x, _position.y + _cornerRadius.y);             // bottom left

		// top right
		Utils::Float2 p2 = p10 + base * _cornerRadius;
		for (unsigned int i = 1; i <= num_points; i++) {
			Utils::Float2 line = base.rotate(i * num_points_inc);
			line.normalise();
			line *= _cornerRadius;
			d.push_back(Utils::Vertex(p10, _boarderColour));
			d.push_back(Utils::Vertex(p2, _boarderColour));
			d.push_back(Utils::Vertex(p10 + line, _boarderColour));

			p2 = p10 + line;
		}
		// top left
		base = Utils::Float2(0, 1);
		p2 = p11 + base * _cornerRadius;
		for (unsigned int i = 1; i <= num_points; i++) {
			Utils::Float2 line = base.rotate(i * num_points_inc);
			line.normalise();
			line *= _cornerRadius;
			d.push_back(Utils::Vertex(p11, _boarderColour));
			d.push_back(Utils::Vertex(p2, _boarderColour));
			d.push_back(Utils::Vertex(p11 + line, _boarderColour));

			p2 = p11 + line;
		}
		// bottom right
		base = Utils::Float2(0, -1);
		p2 = p12 + base * _cornerRadius;
		for (unsigned int i = 1; i <= num_points; i++) {
			Utils::Float2 line = base.rotate(i * num_points_inc);
			line.normalise();
			line *= _cornerRadius;
			d.push_back(Utils::Vertex(p12, _boarderColour));
			d.push_back(Utils::Vertex(p2, _boarderColour));
			d.push_back(Utils::Vertex(p12 + line, _boarderColour));

			p2 = p12 + line;
		}
		// bottom left
		base = Utils::Float2(-1, 0);
		p2 = p13 + base * _cornerRadius;
		for (unsigned int i = 1; i <= num_points; i++) {
			Utils::Float2 line = base.rotate(i * num_points_inc);
			line.normalise();
			line *= _cornerRadius;
			d.push_back(Utils::Vertex(p13, _boarderColour));
			d.push_back(Utils::Vertex(p2, _boarderColour));
			d.push_back(Utils::Vertex(p13 + line, _boarderColour));

			p2 = p13 + line;
		}
	}
	return d;
}

void IGUI_Element::render() const
{
	if (_changed) {
		const Utils::DrawData data = getDrawData();
		_drawBuffer.realoc(data.data(), data.size());
		_changed = false;
	}
	glUseProgram(GUI_Window::elementShader);
	_drawBuffer.bind();
	glDrawArrays(GL_TRIANGLES, 0, _drawBuffer.size());
	_drawBuffer.unBind();
}

void IGUI_Element::setPosition(Utils::Float2 pos, SIZE_MODE mode)
{
	_position = pos;
	if (mode == SIZE_MODE::PIXELS)
		_position /= GUI_Window::windowDimentions;
	_changed = true;
}

void IGUI_Element::setDimentions(Utils::Float2 dims, SIZE_MODE mode)
{
	_dimentions = dims;
	if (mode == SIZE_MODE::PIXELS)
		_dimentions /= GUI_Window::windowDimentions;
	_changed = true;
}

void IGUI_Element::setCornerRadius(float radius, SIZE_MODE mode)
{
	_cornerRadius.x = radius;
	_cornerRadius.y = radius;
	if (mode == SIZE_MODE::PIXELS)
		_cornerRadius /= GUI_Window::windowDimentions;
	_changed = true;
}

static void addQuad(Utils::DrawData& data, const Utils::Float2& offset, const Utils::Float2& dims, const Utils::Float4& colour) {
	data.push_back(Utils::Vertex({ offset.x, offset.y }, colour));
	data.push_back(Utils::Vertex({ offset.x + dims.x, offset.y }, colour));
	data.push_back(Utils::Vertex({ offset.x, offset.y + dims.y }, colour));

	data.push_back(Utils::Vertex({ offset.x + dims.x, offset.y }, colour));
	data.push_back(Utils::Vertex({ offset.x, offset.y + dims.y }, colour));
	data.push_back(Utils::Vertex({ offset.x + dims.x, offset.y + dims.y }, colour));
}