#pragma once
#include <glm.hpp>

struct GeomData {
	glm::vec3 worldPos_;
	// represetns which faces are to be rendered
	unsigned char cubeType_;
	unsigned char textureIndex_;

	GeomData() : worldPos_(), cubeType_(), textureIndex_() { }
};

