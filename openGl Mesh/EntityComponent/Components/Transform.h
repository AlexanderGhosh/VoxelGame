#pragma once
#include <glm.hpp>
#include "../Component.h"

class Transform : public Component {
public:
	Transform();
	Transform(unsigned int id);

	glm::vec3 position;
	float scale;
};