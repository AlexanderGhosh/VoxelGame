#pragma once
#include <glm.hpp>

struct GreedyData {
	unsigned char _xzMin;
	unsigned char _yMin;
	unsigned char _xzMax;
	unsigned char _yMax;
	unsigned short _materialIdx;
	GreedyData() : _xzMin(), _yMin(), _xzMax(), _yMax(), _materialIdx() { }

	void setMin(const glm::vec3& pos) {
		assert(glm::all(glm::greaterThanEqual(pos, glm::vec3(0))));
		unsigned int x = pos.x;
		unsigned int y = pos.y;
		unsigned int z = pos.z;
		_xzMin = x << 4;
		_xzMin |= z;
		_yMin = y;
	}

	glm::vec3 getMin() const {
		glm::vec3 res(0);
		res.x = _xzMin >> 4;
		res.z = _xzMin & 0xf;
		res.y = _yMin;
		return res;
	}

	void setMax(const glm::vec3& pos) {
		assert(glm::all(glm::greaterThanEqual(pos, glm::vec3(0))));
		unsigned int x = pos.x;
		unsigned int y = pos.y;
		unsigned int z = pos.z;
		_xzMax = x << 4;
		_xzMax |= z;
		_yMax = y;
	}

	glm::vec3 getMax() const {
		glm::vec3 res(0);
		res.x = _xzMax >> 4;
		res.z = _xzMax & 0xf;
		res.y = _yMax;
		return res;
	}
};