#pragma once
#include <glm.hpp>
#include "../Component.h"


namespace Components {
	class Transform : public Component {
	public:
		Transform();
		Transform(unsigned int id);

		// world sclaed position (not scaled by scale but by voxel scale)
		glm::vec3 position;
		glm::vec3 scale;

		glm::mat4 getModel() const;
	};
}