#pragma once
#include "../Entity.h"
#include <glm.hpp>

namespace Components {
	class Transform;
	class Camera;
	class FlightControls;
	class BoxCollider;
}
class ChunkColumn;

namespace Entities {
	class Player : public Entity {
	private:
		Components::Transform* _transform;
		Components::Camera* _camera;
		Components::FlightControls* _controls;
		Components::BoxCollider* _collider;
		bool _noClip;
	public:
		Player();
		// if true then collision is off
		Player(bool noClip);
		const glm::mat4 getViewMatrix() const;
		void setPosition(const glm::vec3& pos);

		// Returns the world chunk position unscaled
		const glm::ivec2 getChunkPosition() const;

		const glm::vec3& getViewDirection() const;

		const glm::vec3& getPosition() const;
		// will check if colliding before processing movement
		void processKeys(const std::array<bool, 1024>& keysPressed, const float deltaTime, const std::list<ChunkColumn*>& neighbours);
		void processMouse(const glm::vec2& mouseOffsets);

		void start() override;
	};
};