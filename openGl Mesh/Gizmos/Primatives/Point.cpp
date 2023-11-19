#include "Point.h"
#include "../../Helpers/Constants.h"
#include "../../Shaders/Shader.h"

Gizmo::Point::Point() : IShape()
{
	shader = &SHADERS[POINT_GIZMO];
}

Gizmo::Point::Point(const glm::vec3& pos, const glm::vec3& col) : Point() {
	this->position = pos;
	this->colour = col;
}

void Gizmo::Point::draw(const glm::mat4& pv) {
	bindShader(pv);
	drawGeneral();
}

void Gizmo::Point::cleanUp() {

}

Gizmo::Types Gizmo::Point::getType() const
{
	return Types::Point;
}