#pragma once
#include "../IShape.h"

namespace Gizmo {
	class Point :
		public IShape
	{
	public:
		Point();
		Point(const glm::vec3& pos, const glm::vec3& col);
		~Point() = default;
		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;
	};
}