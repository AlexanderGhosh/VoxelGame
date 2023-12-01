#pragma once
#include "../Entity.h"
#include <glm.hpp>

namespace Components {
	class Transform;
	class RigidBody;
	class RenderMesh;
}

namespace Entities {
	class TestObject : public Entity
	{
	public:
		TestObject();
		void start() override;
		void setPosition(const glm::vec3& pos);
	private:
		Components::Transform* _transform;
		Components::RenderMesh* _renderMesh;
	};
}