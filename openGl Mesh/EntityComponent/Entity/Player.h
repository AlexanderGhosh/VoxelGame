#pragma once
#include "../Entity.h"
#include <glm.hpp>

namespace Components {
	class Transform;
	class Camera;
	class FlightControls;
}

namespace Entities {
	class Player : public Entity {
	private:
		Components::Transform* transform;
		Components::Camera* camera;
		Components::FlightControls* controls;
	public:
		Player();
		const glm::mat4 getViewMatrix() const;

		void start() override;
	};
};