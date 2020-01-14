#pragma once

#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

enum Camera_Movement {
FORWARD,
BACKWARD,
LEFT,
RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 6.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f; // the field of view

class Camera {
public:
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0), GLfloat yaw = YAW,
		GLfloat pitch = PITCH) : front(glm::vec3(0, 0, -1)), movementSpeed(SPEED), mouseSensitivaty(SENSITIVITY), zoom(ZOOM) {
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw,
		GLfloat pitch) : front(glm::vec3(0, 0, -1)), movementSpeed(SPEED), mouseSensitivaty(SENSITIVITY), zoom(ZOOM) {
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}
	glm::vec3 GetRight() {
		return right;
	}

	void ProcessMovement(Camera_Movement direction, GLfloat deltaTime) {
		GLfloat velocity = this->movementSpeed * deltaTime;
		if (direction == FORWARD) {
			this->position += this->front * velocity;
		}
		if (direction == BACKWARD) {
			this->position -= this->front * velocity;
		}
		if (direction == LEFT) {
			this->position -= this->right * velocity;
		}
		if (direction == RIGHT) {
			this->position += this->right * velocity;
		}
	}

	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true) {
		xOffset *= this->mouseSensitivaty;
		yOffset *= this->mouseSensitivaty;

		this->yaw += xOffset;
		this->pitch += yOffset;// up down

		if (constrainPitch) {
			if (this->pitch > 89.0f) {
				this->pitch = 89.0f;
			}
			else if (this->pitch < -89.0f) {
				this->pitch = -89.0f;
			}
		}
		this->updateCameraVectors();
	}
	GLfloat GetZoom() {
		return this->zoom;
	}
	glm::vec3 GetPosition() {
		return this->position;
	}
	glm::vec3 GetFront() {
		return this->front;
	}
	void setPosition(glm::vec3 position) {
		this->position = position;
	}
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 worldUp;
	GLfloat mouseSensitivaty;
	GLfloat pitch;
	GLfloat yaw;
	GLfloat movementSpeed;
	GLfloat zoom;
	
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
};