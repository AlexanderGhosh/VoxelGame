#pragma once
#include <iostream>
#include "GL/glew.h"
#include <glm.hpp>
#include <array>
#include <vector>
#include "Buffer.h"
#include "Textures/Texture.h"

struct FaceMesh {
	std::array<glm::vec3, 6> vertices;
	std::array<glm::vec3, 6> normals;
	std::array<glm::vec3, 6> texCoords;
	Structure data_s;
	GLuint lightLevel;
	glm::vec3 position;
	Texture texture;
	FaceMesh() {
		vertices = std::array<glm::vec3, 6>();
		normals = std::array<glm::vec3, 6>();
		texCoords = std::array<glm::vec3, 6>();
		data_s = Structure();
		position = glm::vec3();
		texture = Texture();
	}
	FaceMesh(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC, GLuint lightLv, Structure structure, Texture textur = Texture());
};

struct Mesh {
	void getVertices(GLboolean vec3 = GL_FALSE);
	Structure data_s;
	std::vector<FaceMesh> faces;
	glm::vec3 position;
};

struct BlockMesh : Mesh {
	GLboolean addFace(FaceMesh& face, GLboolean force = GL_TRUE, glm::vec3 pos = glm::vec3());
};
struct ChunkMesh : Mesh {

};



