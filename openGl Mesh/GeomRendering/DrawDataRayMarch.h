#pragma once
#include <glm.hpp>

class SSBOBuffer;

class DrawDataRayMarch {
public:
	SSBOBuffer* buffer;
	glm::vec3 drawOrigin;

	DrawDataRayMarch() : buffer(), drawOrigin() { }
	DrawDataRayMarch(SSBOBuffer* buffer, glm::vec3 drawOrigin) : buffer(buffer), drawOrigin(drawOrigin) { }
};