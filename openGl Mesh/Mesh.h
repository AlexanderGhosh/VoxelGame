#pragma once
#include <iostream>
#include "GL/glew.h"
#include "Buffer.h"
#include <glm.hpp>
#include <array>
#include <vector>

struct MeshFace {
	std::array<glm::vec3, 6> vertices;
	std::array<glm::vec3, 6> normals;
	std::array<glm::vec3, 6> texCoords;
	Structure data_s;
	
	GLuint lightLevel;

	MeshFace(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC, GLuint lightLv, Structure structure);
};
struct BlockMesh {
	std::vector<MeshFace> faces;
	glm::vec3 position;
	Structure data_s;
	GLboolean addFace(MeshFace& face, GLboolean force = GL_TRUE);
	void getVertices(GLboolean vec3 = GL_FALSE);
};
struct ChunkMesh {

};

