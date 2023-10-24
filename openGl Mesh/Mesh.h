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
		std::vector<glm::vec3> vertices;
		std::array<glm::vec3, 6> normals;
		std::array<glm::vec3, 6> texCoords;
		glm::vec3 position;
		glm::vec3 rotation;
		Buffer* buffer;
		Texture* texture;
		std::vector<Buffer*> comboOf;
		// Physics::BoxCollider collider;
		FaceMesh() : vertices(), normals(), texCoords(), position(), buffer(), texture(), comboOf(), rotation() {
			vertices = std::vector<glm::vec3>();
			normals = std::array<glm::vec3, 6>();
			texCoords = std::array<glm::vec3, 6>();
			position = glm::vec3(3, 2, 0);
			buffer = new Buffer();
			texture = new Texture("grass", GL_TRUE);
			rotation = { 0, 0, 0 };
			// collider = Physics::BoxCollider(position, 1);
		}
		FaceMesh(Buffer* buffer, Texture* texture, glm::vec3 pos = { 3, 2, 0 }) : FaceMesh() {
			position = pos;
			this->buffer = buffer;
			this->texture = texture;
			// texture = new Texture("grass", GL_FALSE);
			rotation = { 0, 0, 0 };
		}
		FaceMesh(GLboolean loadTex);
		FaceMesh(std::vector<glm::vec3>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC);
		void setupBufferStructure(GLboolean is3D = GL_TRUE);
		void setPosition(glm::vec3 position);
		void setRotation(glm::vec3 rotation);
		void setTexture(std::string name);
		void setTexture(Texture* texture);
		void bindTexture();
		void unBindTexture();
		Buffer* getBuffer();
		void cleanUp();
		bool operator <= (const FaceMesh& mesh);
		void operator() (FaceMesh* mesh, glm::vec3 pos);
		void operator() (Buffer*, Texture*, glm::vec3 pos);
		bool isCombo();
		Buffer* getCombo(int index);
		std::vector<GLushort> createIncides();
		// Physics::BoxCollider& getCollider();
	};
	FaceMesh& operator+=(FaceMesh& mesh1, const FaceMesh& mesh2);

	struct BlockMesh {
		std::vector<FaceMesh*> faces;
		glm::vec3 position;
		Buffer* buffer;
		Texture* texture;
		BlockMesh() {
			faces = std::vector<FaceMesh*>();
			position = { 0, 0, 0 };
			buffer = new Buffer();
			texture = new Texture("grass", GL_FALSE);
		}
		void addFace(FaceMesh* face, glm::vec3 pos = glm::vec3());
		void setPosition(glm::vec3 position);

		void setTexture(std::string name);
		void setTexture(Texture* texture);
		void bindTexture();
	};
};




