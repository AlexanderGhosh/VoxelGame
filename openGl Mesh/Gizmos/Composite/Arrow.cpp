#include "Arrow.h"

Gizmo::Arrow::Arrow() : IShape(), lines()
{
}

Gizmo::Arrow::Arrow(const glm::vec3& start, const glm::vec3& end) : Arrow()
{
	lines[0] = Line(start, end, true);
	const const float& size = 0.2f;
	const glm::vec3 d = glm::normalize(end - start);

	glm::vec3 axis(0, 0, size);
	if (d.z != 0) {
		axis = glm::vec3(size, 0, 0);
	}

	const glm::vec3 s1 = end - size * d + glm::vec3(0, 0, size);
	const glm::vec3 s2 = end - size * d - glm::vec3(0, 0, size);

	lines[1] = Line(s1, end, true);
	lines[2] = Line(s2, end, true);
}

void Gizmo::Arrow::draw(const glm::mat4& pv)
{
	for (auto& line : lines) {
		line.draw(pv);
	}
}

void Gizmo::Arrow::cleanUp()
{
	for (auto& line : lines) {
		line.cleanUp();
	}
}

Gizmo::Types Gizmo::Arrow::getType() const
{
	return Types::Arrow;
}

void Gizmo::Arrow::setPosition(const glm::vec3& pos)
{
	const glm::vec3 delta = pos - position;
	for (auto& l : lines) {
		l.setPosition(l.getPosition() + delta);
	}
}

void Gizmo::Arrow::setColour(const glm::vec3& colour)
{
	IShape::setColour(colour);
	for (auto& line : lines) {
		line.setColour(colour);
	}
}
