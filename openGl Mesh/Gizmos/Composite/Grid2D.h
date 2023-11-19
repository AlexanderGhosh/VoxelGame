#pragma once
#include "../IShape.h"
#include <vector>
#include <glm.hpp>
#include "../Primatives/Line.h"

namespace Gizmo {
	// position is the bottom left
	class Grid2D :
		public IShape
	{
	private:
		std::vector<Line> lines;
		// the horizontal size of each square
		float squareSize;
		// how many squares by (column, row)
		glm::ivec2 dimentions;
		bool xPlane, yPlane;
	public:
		Grid2D();
		Grid2D(const glm::vec3& position, const glm::ivec2& dim, const float size, bool xPlane, bool yPlane);

		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;

		void setColour(const glm::vec3& colour) override;
		void setPosition(const glm::vec3& position) override;
	};
}

