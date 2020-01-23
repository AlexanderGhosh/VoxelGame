#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <array>
#include <vector>
// #include "Game/World/constants.h"
#include "Buffer.h"
#include "Textures/Texture.h"
// #include "Game/World/Physics.h"
namespace Mesh {
	struct FaceMesh {
		std::array<glm::vec3, 6> vertices;
		std::array<glm::vec3, 6> normals;
		std::array<glm::vec3, 6> texCoords;
		glm::vec3 position;
		glm::vec3 rotation;
		Buffer* buffer;
		Texture* texture;
		// Physics::BoxCollider collider;
		FaceMesh() {
			vertices = std::array<glm::vec3, 6>();
			normals = std::array<glm::vec3, 6>();
			texCoords = std::array<glm::vec3, 6>();
			position = glm::vec3(3, 2, 0);
			buffer = new Buffer();
			texture = new Texture("grass", GL_TRUE);
			rotation = { 0, 0, 0 };
			// collider = Physics::BoxCollider(position, 1);
		}
		FaceMesh(Buffer* buffer) {
			position = glm::vec3(3, 2, 0);
			this->buffer = buffer;
			texture = new Texture("grass", GL_FALSE);
			rotation = { 0, 0, 0 };
		}
		FaceMesh(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC);
		void setupBufferStructure(GLboolean is3D = GL_TRUE);
		void setPosition(glm::vec3 position);
		void setRotation(glm::vec3 rotation);
		void setTexture(std::string name);
		void setTexture(Texture* texture);
		void bindTexture();
		void unBindTexture();
		Buffer* getBuffer();
		// Physics::BoxCollider& getCollider();
	};

	struct BlockMesh {
		std::vector<FaceMesh> faces;
		glm::vec3 position;
		Buffer* buffer;
		Texture* texture;
		BlockMesh() {
			faces = std::vector<FaceMesh>();
			position = { 0, 0, 0 };
			buffer = new Buffer();
			texture = new Texture("grass", GL_FALSE);
		}
		void addFace(const FaceMesh& face, glm::vec3 pos = glm::vec3());
		void setPosition(glm::vec3 position);

		void setTexture(std::string name);
		void setTexture(Texture* texture);
		void bindTexture();
	};
};




