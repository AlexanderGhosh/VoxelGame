#pragma once
#include "../IShape.h"
#include <vector>
#include <array>
#include "../Primatives/Line.h"
#include "../Primatives/Point.h"

namespace Gizmo {
	class Triangle :
		public IShape
	{
	private:
		std::array<Line, 3> lines;
		std::vector<Point> points;
	public:
		Triangle();
		Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, bool drawPoints);
		~Triangle() = default;

		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;

		void setPosition(const glm::vec3& pos) override;
		void setColour(const glm::vec3& colour) override;
	};
}
