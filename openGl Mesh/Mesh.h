#pragma once
#include "GL/glew.h"
#include <glm.hpp>
#include <array>
#include <vector>

struct MeshFace {
	std::array<glm::vec3, 6> vertices;
	std::array<glm::vec3, 6> normals;
	std::array<glm::ivec2, 6> texCoords;
	glm::vec3 rotation;
	
	GLuint lightLevel;

	MeshFace(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::ivec2, 6 > texC, glm::vec3 rot, GLuint lightLv);
};
struct BlockMesh {
	std::vector<MeshFace> faces;
	glm::vec3 position;
	GLboolean addFace(MeshFace& face);
	std::vector<GLfloat> getVertices();
};

