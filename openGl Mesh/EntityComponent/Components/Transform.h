#pragma once
#include <glm.hpp>
#include "../Component.h"

class Transform : public Component {
private:
	glm::vec3 position;
	float scale;
public:
	Transform();
	Transform(const glm::vec3& pos, const float scale);

	const glm::vec3& getPosition() const;
	const float& getScale() const;

	void setPosition(const glm::vec3& pos);
	void setScale(const float& scale);
};