#pragma once
#include <glm.hpp>
#include "../Textures/Texture.h"

class SSBOBuffer;

class DrawDataRayMarch {
public:
	Texture noiseData;
	glm::vec3 drawOrigin;
	float maxHeight;

	DrawDataRayMarch() : noiseData(), drawOrigin(), maxHeight() { }
	DrawDataRayMarch(Texture buffer, glm::vec3 drawOrigin, float maxHeight) : noiseData(buffer), drawOrigin(drawOrigin), maxHeight(maxHeight) { }
};