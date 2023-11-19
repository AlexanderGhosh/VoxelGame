#pragma once
#include "../IShape.h"
#include <array>
#include "../Primatives/Line.h"

namespace Gizmo {
	class Arrow :
		public IShape
	{
	private:
		std::array<Line, 3> lines;
	public:
		Arrow();
		Arrow(const glm::vec3& start, const glm::vec3& end);
		~Arrow() = default;

		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;

		void setPosition(const glm::vec3& pos) override;
		void setColour(const glm::vec3& colour) override;
	};
}

