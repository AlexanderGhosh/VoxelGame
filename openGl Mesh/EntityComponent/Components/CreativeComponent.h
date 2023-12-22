#pragma once
#include <array>
#include <ext/vector_float3.hpp>
#include "../Component.h"

namespace Components {
	class Transform;
	class RigidBody;
	class CreativeComponent : public Component
	{
	private:
		enum Mode {
			FLY, WALK
		} _mode;
		Transform* _transform;
		RigidBody* _rigidbody;
		float _speed;
		bool _isGrounded;
		bool _justJumped;
		// in fames
		int _jumpCooldown;
		// in frames
		const int JUMP_COOLDOWN_MAX = 300; 
	public:
		CreativeComponent(bool hasCollision = true);

		// fwd is not front it is the direction that is considered forward
		void processKeys(const std::array<bool, 1024>& keysPressed, const glm::vec3& fwd, const float deltaTime);

		void update(const float dt) override;

		void setTransform(Transform* transform);
		void setRigidbody(RigidBody* rigidbody);
		void setSpeed(const float speed);
	};
}

