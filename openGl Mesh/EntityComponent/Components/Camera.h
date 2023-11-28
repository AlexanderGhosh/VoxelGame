#pragma once

#include <vector>

#include <glad/glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include "../../Helpers/Constants.h"
#include "../Component.h"


namespace Components {
	class Camera : public Component {
	public:
		Camera();

		const glm::mat4 getViewMatrix(const glm::vec3& eye) const;

		void processMouseMovement(float xOffset, float yOffset);

		const glm::vec3& getFront() const;
		const glm::vec3& getUp() const;
		const glm::vec3& getRight() const;
	private:
		void updateCameraVectors();

		glm::vec3 _front;
		glm::vec3 _right;
		glm::vec3 _up;
		float _mouseSensitivaty;
		float _pitch;
		float _yaw;


		const float YAW = -90.0f;
		const float PITCH = 0.0f;
		const float SENSITIVITY = 0.25f;
	};
}