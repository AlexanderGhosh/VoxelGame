#pragma once
#include "../World/constants.h"
class BoxCollider
{
public:
	BoxCollider();
	BoxCollider(glm::vec3 dimentions, glm::vec3 position);
	GLboolean isColliding(BoxCollider& collider);
	void setPosition(glm::vec3 position);
protected:
	glm::vec3 dimentions;
	glm::vec3 min, max;
	glm::vec3 position;
private:
	void updateMinMax();
};

