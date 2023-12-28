#pragma once
#include <array>
#include <ext/vector_float3.hpp>
#include "../Component.h"

namespace Components {
	class Transform;
	class RigidBody;
	class Camera;
	class CreativeComponent : public Component
	{
	private:
		enum Mode {
			FLY, WALK
		} _mode;
		Transform* _transform;
		RigidBody* _rigidbody;
		Camera* _camera;

		float _speed;
		bool _isGrounded;
		bool _justJumped;
		// in fames
		int _jumpCooldown;
		// in frames
		const int JUMP_COOLDOWN_MAX = 60; 

		void setFly();
		void setWalk();
	public:
		CreativeComponent(bool hasCollision = true);

		void start() override;
		void update(const float dt) override;
		void onKeyPress(KeyEventInfo info) override;
		void onCollision(CollisionEventInfo info) override;

		void setTransform(Transform* transform);
		void setRigidbody(RigidBody* rigidbody);
		void setSpeed(const float speed);
	};
}

