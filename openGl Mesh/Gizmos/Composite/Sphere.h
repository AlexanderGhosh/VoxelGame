#pragma once
#include "../IShape.h"
#include <array>
#include "../Primatives/Circle.h"

namespace Gizmo {
	class Sphere :
		public IShape
	{
	private:
		float radius;
		bool updated;
		std::array<Circle, 3> cirlces;
	public:
		Sphere();
		Sphere(const glm::vec3& pos, const glm::vec3& col);
		~Sphere() = default;
		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;
		void setRadius(const float& r);
		void setPosition(const glm::vec3& pos) override;
		void setColour(const glm::vec3& colour) override;
	};
}

