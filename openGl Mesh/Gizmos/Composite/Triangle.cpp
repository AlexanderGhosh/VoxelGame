#include "Triangle.h"
#include "../Primatives/Point.h"

Gizmo::Triangle::Triangle() : IShape(), lines(), points()
{
}

Gizmo::Triangle::Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, bool drawPoints) : Triangle()
{
	lines[0] = Line(p1, p2, true);
	lines[1] = Line(p2, p3, true);
	lines[2] = Line(p3, p1, true);

	if (drawPoints) {
		points.push_back(Point(p1, colour));
		points.push_back(Point(p2, colour));
		points.push_back(Point(p3, colour));
		for (auto& p : points) {
			p.setThickness(1);
		}
	}
}

void Gizmo::Triangle::draw(const glm::mat4& pv)
{
	for (auto& line : lines) {
		line.draw(pv);
	}
	for (auto& point : points) {
		point.draw(pv);
	}
}

void Gizmo::Triangle::cleanUp()
{
	for (auto& line : lines) {
		line.cleanUp();
	}
	for (auto& point : points) {
		point.cleanUp();
	}
}

Gizmo::Types Gizmo::Triangle::getType() const
{
	return Types::Triangle;
}

void Gizmo::Triangle::setPosition(const glm::vec3& pos)
{
	const glm::vec3 delta = pos - position;
	for (auto& l : lines) {
		l.setPosition(l.getPosition() + delta);
	}
}

void Gizmo::Triangle::setColour(const glm::vec3& colour)
{
	IShape::setColour(colour);
	for (auto& line : lines) {
		line.setColour(colour);
	}
	for (auto& point : points) {
		point.setColour(colour);
	}
}