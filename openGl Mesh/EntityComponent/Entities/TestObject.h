#pragma once
#include "../Entity.h"
#include <glm.hpp>

namespace Components {
	class Transform;
	class RigidBody;
}

namespace Entities {
	class TestObject : public Entity
	{
	public:
		TestObject();
		void start() override;
		void setPosition(const glm::vec3& pos);
		Components::RigidBody* getRigidBody();
	private:
		Components::Transform* _transform;
		Components::RigidBody* _rigidbody;
	};
}

