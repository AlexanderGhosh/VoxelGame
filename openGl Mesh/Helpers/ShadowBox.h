#pragma once
#include <glm.hpp>

class ShadowBox {
public:
	static glm::mat4 getLSM(const glm::mat4& view, const glm::vec3& lightPos);
};
