#include "Line.h"
#include "../../Helpers/Constants.h"
#include "../../Shaders/Shader.h"


Gizmo::Line::Line() : IShape(), start(), end()
{
	shader = &SHADERS[LINE_GIZMO];
}

Gizmo::Line::Line(const glm::vec3& origin, const glm::vec3& dir, const float& length) : Line()
{
	start = origin;
	end = origin + glm::normalize(dir) + length;
}

Gizmo::Line::Line(const glm::vec3& from, const glm::vec3& to) : Line()
{
	start = from;
	end = to;
}

void Gizmo::Line::draw(const glm::mat4& pv) {
	bindShader(pv);
	shader->setValue("start", start);
	shader->setValue("end", end);
	drawGeneral();
}

void Gizmo::Line::cleanUp() {

}

Gizmo::Types Gizmo::Line::getType() const
{
	return Types::Line;
}

void Gizmo::Line::setStart(const glm::vec3& origin)
{
	this->start = origin;
}

void Gizmo::Line::setEnd(const glm::vec3& end)
{
	this->end = end;
}

void Gizmo::Line::setPosition(const glm::vec3& pos)
{
	IShape::setPosition(pos);
	const glm::vec3 delta = pos - position;
	start += delta;
	end += delta;
}