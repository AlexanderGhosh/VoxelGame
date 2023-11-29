#pragma once
#include <glm.hpp>
#include "../Component.h"


namespace Components {
	class Transform : public Component {
	public:
		Transform();
		Transform(unsigned int id);

		glm::vec3 position;
		glm::vec3 scale;
	};
}