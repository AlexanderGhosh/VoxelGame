#pragma once
#include "../IShape.h"

namespace Gizmo {
	class Circle :
		public IShape
	{
	private:
		float radius;
		glm::vec3 up;
	public:
		Circle();
		Circle(const glm::vec3& pos, const glm::vec3& col);
		~Circle() = default;
		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;
		void setRadius(const float& r);
		void setUp(const glm::vec3& up);
	};
}