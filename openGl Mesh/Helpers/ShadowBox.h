#pragma once
#include <glm.hpp>

class Camera;
class ShadowBox {
public:
	static glm::mat4 getLSM(Camera& camera, const glm::mat4& proj, const glm::vec3& lightPos);
};
