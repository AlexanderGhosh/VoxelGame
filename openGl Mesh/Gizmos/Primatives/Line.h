#pragma once
#include "../IShape.h"

namespace Gizmo {
	class Line :
		public IShape
	{
	private:
		glm::vec3 start, end;
	public:
		Line();
		Line(const glm::vec3& from, const glm::vec3& to);
		Line(const glm::vec3& origin, const glm::vec3& dir, const float& length);
		~Line() = default;
		void draw(const glm::mat4& pv) override;
		void cleanUp() override;
		Types getType() const override;
		void setStart(const glm::vec3& left);
		void setEnd(const glm::vec3& right);

		void setPosition(const glm::vec3& pos) override;
	};
}