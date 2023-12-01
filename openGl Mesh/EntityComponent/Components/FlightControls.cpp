#include "FlightControls.h"
#include <glfw3.h>
#include "RigidBody.h"

Components::FlightControls::FlightControls() : Component(), _rigidbody(), _speed()
{
}

Components::FlightControls::FlightControls(const float speed) : FlightControls()
{
	_speed = speed;
}

void Components::FlightControls::processKeys(const std::array<bool, 1024>& keysPressed, const glm::vec3& fwd, const float deltaTime)
{
	const glm::vec3 right = glm::normalize(glm::cross(fwd, glm::vec3(0, 1, 0)));
	glm::vec3 detlaV(0);
	if (keysPressed[GLFW_KEY_W]) {
		detlaV += fwd * _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_S]) {
		detlaV -= fwd * _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_A]) {
		detlaV -= right * _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_D]) {
		detlaV += right * _speed * deltaTime;
	}

	if (keysPressed[GLFW_KEY_SPACE]) {
		detlaV.y += _speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_LEFT_SHIFT]) {
		detlaV.y -= _speed * deltaTime;
	}
	_rigidbody->addVelocity(detlaV);
}

void Components::FlightControls::setRigidBody(RigidBody* rb)
{
	_rigidbody = rb;
}

void Components::FlightControls::setSpeed(const float speed)
{
	_speed = speed;
}
