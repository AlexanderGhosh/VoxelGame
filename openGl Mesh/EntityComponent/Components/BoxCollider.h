#pragma once
#include "../Component.h"
#include <glm.hpp>

namespace Components {
	class Transform;
	class BoxCollider : public Component {
	private:
		Transform* _transform;
	public:
		BoxCollider();

		// true if colliding with box collider
		bool colliding(const BoxCollider* b) const;
		// true if colliding with voxel at given position
		bool colliding(const glm::vec3& pos) const;

		void setTransform(Transform* transform);

		void destroy() override;
	};
}