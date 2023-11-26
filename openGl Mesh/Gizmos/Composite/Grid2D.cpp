#include "Grid2D.h"
#include <glad/glad.h>

Gizmo::Grid2D::Grid2D() : IShape(), lines(), squareSize(), dimentions(), xPlane(), yPlane()
{
}

Gizmo::Grid2D::Grid2D(const glm::vec3& position, const glm::ivec2& dim, const float size, bool xPlane, bool yPlane) : Grid2D()
{
	this->position = position;
	dimentions = dim;
	squareSize = size;
	this->xPlane = xPlane;
	this->yPlane = yPlane;
	lines.reserve(dim.x + dim.y + 2);
	if (yPlane) {
	}
	else if (xPlane) {
		// verticle lines
		for (unsigned int i = 0; i < dim.x + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(0, 0, size);
			Line line(s, s + glm::vec3(0, size * dim.y, 0));
			line.setColour(colour);
			lines.push_back(line);
		}
		// horizontal
		for (unsigned int i = 0; i < dim.y + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(0, size, 0);
			Line line(s, s + glm::vec3(0, 0, size * dim.x));
			line.setColour(colour);
			lines.push_back(line);
		}
	}
	else {
		// verticle lines
		for (unsigned int i = 0; i < dim.x + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(size, 0, 0);
			Line line(s, s + glm::vec3(0, size * dim.y, 0));
			line.setColour(colour);
			lines.push_back(line);
		}
		// horizontal
		for (unsigned int i = 0; i < dim.y + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(0, size, 0);
			Line line(s, s + glm::vec3(size * dim.x, 0, 0));
			line.setColour(colour);
			lines.push_back(line);
		}
	}
}

void Gizmo::Grid2D::draw(const glm::mat4& pv)
{
	glDepthMask(false);
	for (auto& line : lines) {
		line.draw(pv);
	}

	glDepthMask(true);
}

void Gizmo::Grid2D::cleanUp()
{
	for (auto& line : lines) {
		line.cleanUp();
	}
	lines.clear();
}

Gizmo::Types Gizmo::Grid2D::getType() const
{
	return Types::Grid2D;
}

void Gizmo::Grid2D::setColour(const glm::vec3& colour)
{
	IShape::setColour(colour);
	for (auto& line : lines) {
		line.setColour(colour);
	}
}

void Gizmo::Grid2D::setPosition(const glm::vec3& position)
{
	IShape::setPosition(position);

	this->position = position;
	if (yPlane) {
	}
	else if (xPlane) {
		// verticle lines
		for (unsigned int i = 0; i < dimentions.x + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(0, 0, squareSize);
			lines[i].setStart(s);
			lines[i].setEnd(s + glm::vec3(0, squareSize * dimentions.y, 0));
		}
		// horizontal
		for (unsigned int i = 0; i < dimentions.y + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(0, squareSize, 0);
			lines[i + dimentions.x + 1].setStart(s);
			lines[i + dimentions.x + 1].setEnd(s + glm::vec3(0, 0, squareSize * dimentions.x));
		}
	}
	else {
		// verticle lines
		for (unsigned int i = 0; i < dimentions.x + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(squareSize, 0, 0);
			lines[i].setStart(s);
			lines[i].setEnd(s + glm::vec3(0, squareSize * dimentions.y, 0));
		}
		// horizontal
		for (unsigned int i = 0; i < dimentions.y + 1; i++) {
			glm::vec3 s = position + (float)i * glm::vec3(0, squareSize, 0);
			lines[i + dimentions.x + 1].setStart(s);
			lines[i + dimentions.x + 1].setEnd(s + glm::vec3(squareSize * dimentions.x, 0, 0));
		}
	}
}
