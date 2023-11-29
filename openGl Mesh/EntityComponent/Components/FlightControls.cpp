#include "FlightControls.h"
#include <glfw3.h>
#include "Transform.h"

Components::FlightControls::FlightControls() : Component(), _transform(), _speed()
{
}

Components::FlightControls::FlightControls(const float speed) : FlightControls()
{
	_speed = speed;
}

void Components::FlightControls::processKeys(const std::array<bool, 1024>& keysPressed, const glm::vec3& fwd, const float deltaTime)
{
	const glm::vec3 right = glm::normalize(glm::cross(fwd, glm::vec3(0, 1, 0)));

	if (keysPressed[GLFW_KEY_W]) {
		_transform->position += fwd * _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_S]) {
		_transform->position -= fwd * _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_A]) {
		_transform->position -= right * _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_D]) {
		_transform->position += right * _speed * deltaTime;
	}

	if (keysPressed[GLFW_KEY_SPACE]) {
		_transform->position.y += _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_LEFT_SHIFT]) {
		_transform->position.y -= _speed * deltaTime;
	}
}

void Components::FlightControls::setTransform(Transform* transform)
{
	_transform = transform;
}

void Components::FlightControls::setSpeed(const float speed)
{
	_speed = speed;
}
