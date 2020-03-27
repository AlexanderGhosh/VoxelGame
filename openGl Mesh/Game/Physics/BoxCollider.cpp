#include "BoxCollider.h"
BoxCollider::BoxCollider() {
	dimentions = glm::vec3(1.0f);
	updateMinMax();
}
BoxCollider::BoxCollider(glm::vec3 dimentions, glm::vec3 position) {
	this->position = position;
	this->dimentions = dimentions;
	updateMinMax();
}
GLboolean BoxCollider::isColliding(BoxCollider& collider) {
	auto& a = *this;
	auto& b = collider;
	if (b.position.y == -11 && b.position.x == 0 && b.position.z == 0) {
		if (glm::distance(a.position, b.position) < 1.0f) {
			int g = 0;
		}
	}

	return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			 (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			 (a.min.z <= b.max.z && a.max.z >= b.min.z);
}
void BoxCollider::updateMinMax() {
	auto dim = dimentions / 2.0f;
	min = position - dim;
	max = position + dim;
}
void BoxCollider::setPosition(glm::vec3 position) {
	this->position = position;
	updateMinMax();
}