#pragma once
#include <glm.hpp>

struct GreedyData {
	glm::vec3 _corner0;
	glm::vec3 _corner1;
	glm::vec3 _corner2;
	glm::vec3 _corner3;
	glm::vec3 _normal;

	unsigned int _materialIdx;

	GreedyData() : _corner0(), _corner1(), _corner2(), _corner3(), _normal(), _materialIdx() { }
};