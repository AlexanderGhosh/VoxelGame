#pragma once
#include <glm.hpp>

struct GeomData {
	glm::vec3 worldPos_;
	// represetns which faces are to be rendered
	unsigned int cubeType_;
	unsigned int textureIndex_;

	GeomData() : worldPos_(), cubeType_(), textureIndex_() { }
};

