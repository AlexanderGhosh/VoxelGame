#pragma once
#include "../Entity.h"
#include <glm.hpp>

namespace Components {
	class Transform;
	class Camera;
	class CreativeComponent;
	class FlightControls;
	class RigidBody;
}
class ChunkColumn;

namespace Entities {
	class Player : public Entity {
	private:
		Components::Transform* _transform;
		Components::Camera* _camera;
		Components::CreativeComponent* _controls;
		Components::RigidBody* _rigidbody;
		bool _noClip;
	public:
		Player();
		// if true then collision is off
		Player(bool noClip);
		const glm::mat4 getViewMatrix() const;
		void setPosition(const glm::vec3& pos);

		Components::RigidBody* getRigidBody();
		// Returns the world chunk position unscaled
		const glm::ivec2 getChunkPosition() const;

		const glm::vec3& getViewDirection() const;

		const glm::vec3& getPosition() const;
		const glm::vec3& getFront() const;
		// will check if colliding before processing movement
		void processMouse(const glm::vec2& mouseOffsets);

		void start() override;
	};
};