#pragma once
#include "GL/glew.h"
#include <glm.hpp>
#include <array>
#include <vector>
#include "Buffer.h"
#include "Textures/Texture.h"
namespace Mesh {
	struct FaceMesh {
		std::array<glm::vec3, 6> vertices;
		std::array<glm::vec3, 6> normals;
		std::array<glm::vec3, 6> texCoords;
		glm::vec3 position;
		Buffer buffer;
		Texture texture;
		FaceMesh() {
			vertices = std::array<glm::vec3, 6>();
			normals = std::array<glm::vec3, 6>();
			texCoords = std::array<glm::vec3, 6>();
			position = glm::vec3(3, 2, 0);
			buffer = Buffer();
			texture = Texture("grass", GL_TRUE);
		}
		FaceMesh(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC);
		void setupBufferStructure(GLboolean is3D = GL_TRUE);
		void setPosition(glm::vec3 position);

		void setTexture(std::string name);
		void bindTexture();
		void unBindTexture();
	};

	struct BlockMesh {
		std::vector<FaceMesh> faces;
		glm::vec3 position;
		Buffer buffer;
		Texture texture;
		BlockMesh() {
			faces = std::vector<FaceMesh>();
			position = { 0, 0, 0 };
			buffer = Buffer();
			texture = Texture("grass", GL_FALSE);
		}
		void addFace(FaceMesh& face, glm::vec3 pos = glm::vec3());
		void setPosition(glm::vec3 position);

		void setTexture(std::string name);
		void bindTexture();
	};
};



