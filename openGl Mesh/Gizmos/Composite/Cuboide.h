#pragma once
#include "../IShape.h"
#include <gtc/quaternion.hpp>
#include <array>
#include "../Primatives/Line.h"

namespace Gizmo {
	class Cuboide :
		public IShape
	{
	private:
		glm::vec3 dimensions;
		std::array<Line, 12> lines;
		glm::quat rotation;
	public:
		Cuboide();
		Cuboide(const glm::vec3& pos, const glm::vec3& col);
		~Cuboide() = default;
		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;
		void setDimentions(const glm::vec3& dim);
		void setRotation(const glm::quat& rot);
	};
}
