#pragma once
#include <glm.hpp>

struct GeomData {
	unsigned char xz;
	unsigned char y;
	// 1 in the slot that is to be shown slot order can be found in constants.h
	unsigned char cubeType_;
	unsigned char textureIndex_;


	// represetns which faces are to be rendered

	GeomData() : xz(), y(), cubeType_(), textureIndex_() { }

	void setPos(const glm::vec3& pos) {
		assert(glm::all(glm::greaterThanEqual(pos, glm::vec3(0))));
		unsigned int x = pos.x;
		unsigned int y = pos.y;
		unsigned int z = pos.z;
		xz = x << 4;
		xz |= z;
		this->y = y;
	}
	glm::vec3 getPos() const {
		glm::vec3 res(0);
		res.x = xz >> 4;
		res.z = xz & 0xf;
		res.y = y;
		return res;
	}
};

