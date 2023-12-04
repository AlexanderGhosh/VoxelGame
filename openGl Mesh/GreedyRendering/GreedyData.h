#pragma once
#include <glm.hpp>

struct GreedyData {
	glm::vec3 _corner0;
	glm::vec3 _corner1;
	glm::vec3 _corner2;
	glm::vec3 _corner3;

	unsigned int _materialIdx;

	GreedyData() : _corner0(), _corner1(), _corner2(), _corner3(), _materialIdx() { }
};