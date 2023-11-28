#include "Camera.h"
#include "../../Helpers/Constants.h"

using namespace Components;

Camera::Camera() : Component(), _front(0, 0, 1), _right(-1, 0, 0), _up(0, 1, 0), _mouseSensitivaty(), _pitch(), _yaw()
{
}

const glm::mat4 Camera::getViewMatrix(const glm::vec3& eye) const
{
	return glm::lookAt(eye, eye + _front, _up);
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
	xOffset *= SENSITIVITY;
	yOffset *= SENSITIVITY;

	_yaw += xOffset;
	_pitch += yOffset; // up down

	if (_pitch > 89.0f) {
		_pitch = 89.0f;
	}
	else if (_pitch < -89.0f) {
		_pitch = -89.0f;
	}
	updateCameraVectors();
}

void Camera::updateCameraVectors() {
	_front.x = cos(glm::radians(_yaw)) * cosf(glm::radians(_pitch));
	_front.y = sin(glm::radians(_pitch));
	_front.z = sin(glm::radians(_yaw)) * cosf(glm::radians(_pitch));

	_front = glm::normalize(_front);
	_right = glm::normalize(glm::cross(_front, glm::vec3(0, 1, 0)));
	_up = glm::normalize(glm::cross(_right, _front));
}

const glm::vec3& Camera::getFront() const
{
	return _front;
}
const glm::vec3& Camera::getUp() const
{
	return _up;
}
const glm::vec3& Camera::getRight() const
{
	return _right;
}