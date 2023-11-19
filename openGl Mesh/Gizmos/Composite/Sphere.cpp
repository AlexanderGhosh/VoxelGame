#include "Sphere.h"

Gizmo::Sphere::Sphere() : IShape(), radius(1), cirlces(), updated(true)
{
	cirlces[1].setUp(glm::vec3(1, 0, 0));
	cirlces[2].setUp(glm::vec3(0, 0, 1));
}

Gizmo::Sphere::Sphere(const glm::vec3& pos, const glm::vec3& col) : Sphere()
{
	this->position = pos;
	this->colour = col;
}

void Gizmo::Sphere::draw(const glm::mat4& pv)
{
	for (char i = 0; i < 3; i++) {
		if (updated) {
			cirlces[i].setColour(this->colour);
			cirlces[i].setPosition(this->position);
			cirlces[i].setRadius(this->radius);
		}
		cirlces[i].draw(pv);
	}
}

void Gizmo::Sphere::cleanUp()
{
	for (char i = 0; i < 3; i++) {
		cirlces[i].cleanUp();
	}
}

Gizmo::Types Gizmo::Sphere::getType() const
{
	return Types::Sphere;
}

void Gizmo::Sphere::setRadius(const float& r)
{
	updated = true;
	this->radius = r;
}

void Gizmo::Sphere::setPosition(const glm::vec3& pos)
{
	updated = true;
	IShape::setPosition(pos);
}

void Gizmo::Sphere::setColour(const glm::vec3& colour)
{
	updated = true;
	IShape::setColour(colour);
}