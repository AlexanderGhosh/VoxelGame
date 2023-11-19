#include "Cuboide.h"
#include <glad/glad.h>
#include "../../Helpers/Constants.h"

Gizmo::Cuboide::Cuboide() : IShape(), dimensions(0.5), lines(), rotation()
{
}

Gizmo::Cuboide::Cuboide(const glm::vec3& pos, const glm::vec3& col) : Cuboide()
{
	this->position = pos;
	this->colour = col;
}

void Gizmo::Cuboide::draw(const glm::mat4& pv) {
	bindShader(pv);
	glm::mat4 model = glm::translate(glm::mat4(1), position);
	model = glm::mat4(rotation) * model;
	model = glm::scale(model, dimensions);

	glPointSize(thickness);
	glLineWidth(thickness);

	for (short i = 0; i < lines.size(); i++) {
		lines[i].draw(pv);
	}


	glPointSize(1);
	glLineWidth(1);
}

void Gizmo::Cuboide::cleanUp() {

}

Gizmo::Types Gizmo::Cuboide::getType() const
{
	return Types::Cuboide;
}

void Gizmo::Cuboide::setDimentions(const glm::vec3& dim)
{
	this->dimensions = dim;
}

void Gizmo::Cuboide::setRotation(const glm::quat& rot)
{
	rotation = rot;
}