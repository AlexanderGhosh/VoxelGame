#include "Circle.h"
#include "../../Helpers/Constants.h"
#include "../../Shaders/Shader.h"

Gizmo::Circle::Circle() : IShape(), radius(0.5), up(0, 1, 0)
{
	shader = &SHADERS[CIRCLE_GIZMO];
}

Gizmo::Circle::Circle(const glm::vec3& pos, const glm::vec3& col) : Circle()
{
	this->position = pos;
	this->colour = col;
}

void Gizmo::Circle::draw(const glm::mat4& pv) {
	bindShader(pv);
	const float angle = glm::degrees(acosf(glm::dot(glm::vec3(0, 1, 0) , glm::normalize(up))));
	glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), up));
	for (char i = 0; i < 3; i++) {
		float& x = axis[i];
		if (glm::isnan(x) || glm::isinf(x)) {
			x = 0;
		}
	}
	glm::mat4 rot = glm::rotate(glm::mat4(1), angle, axis);
	if (angle == 0) rot = glm::mat4(1);
	shader->setValue("rotation", rot);
	shader->setValue("radius", radius);
	drawGeneral();
}

void Gizmo::Circle::cleanUp() {

}

Gizmo::Types Gizmo::Circle::getType() const
{
	return Types::Circle;
}

void Gizmo::Circle::setRadius(const float& r)
{
	this->radius = r;
}

void Gizmo::Circle::setUp(const glm::vec3& up)
{
	this->up = up;
}