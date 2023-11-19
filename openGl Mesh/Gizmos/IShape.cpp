#include "IShape.h"
#include <glad/glad.h>
#include "../Shaders/Shader.h"

Gizmo::IShape::IShape() : position(0), colour(1), thickness(1), pointVAO(0), shader()
{
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &pointVAO);
	glBindVertexArray(pointVAO);

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);


	int stride = sizeof(glm::vec3);
	// layouts in shader
	// pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Gizmo::IShape::~IShape()
{
	glDeleteVertexArrays(1, &pointVAO);
}

void Gizmo::IShape::bindShader(const glm::mat4& pv) const
{
	shader->bind();
	shader->setValue("pv", pv);
}

void Gizmo::IShape::bindVAO() const
{
	glBindVertexArray(pointVAO);
}

void Gizmo::IShape::unbindVAO() const
{
	glBindVertexArray(0);
}

void Gizmo::IShape::drawGeneral()
{
	shader->setValue("position", position);
	shader->setValue("colour", colour);
	glPointSize(thickness);
	glLineWidth(thickness);
	bindVAO();
	glDrawArrays(GL_POINTS, 0, 1);
	unbindVAO();
	glPointSize(1);
	glLineWidth(1);
}

void Gizmo::IShape::setThickness(const unsigned int size)
{
	thickness = size;
}

const glm::vec3& Gizmo::IShape::getPosition() const
{
	return position;
}

const glm::vec3& Gizmo::IShape::getColour() const
{
	return colour;
}

void Gizmo::IShape::setPosition(const glm::vec3& pos)
{
	this->position = pos;
}

void Gizmo::IShape::setColour(const glm::vec3& colour)
{
	this->colour = colour;
}