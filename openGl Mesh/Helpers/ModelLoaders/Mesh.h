#pragma once
#include <vector>
#include <glm.hpp>
#include <list>

class Mesh {
public:
	Mesh() : vertexPoss(), vertexNorms(), vertexTexCoords(), indices() { }

	std::vector<glm::vec3> vertexPoss;
	std::vector<glm::vec3> vertexNorms;
	std::vector<glm::vec2> vertexTexCoords;
	std::vector<unsigned int> indices;
	std::list<Mesh*> _children;
};