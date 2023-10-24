#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <string>
#include <array>
#include <vector>
// #include "Game/World/constants.h"
// #include "Game/World/Physics.h"

class Buffer;
class Texture;
namespace Mesh {
	struct FaceMesh {
		std::vector<glm::vec3> vertices;
		glm::vec3 normal;
		std::array<glm::vec3, 6> texCoords;
		glm::vec3 position;
		glm::vec3 rotation;
		Buffer* buffer;
		Texture* texture;
		std::vector<Buffer*> comboOf;
		// Physics::BoxCollider collider;

		FaceMesh();
		FaceMesh(std::vector<glm::vec3>vert, glm::vec3 norm, std::array<glm::vec3, 6 > texC);
		void setPosition(glm::vec3 position);
		// void setRotation(glm::vec3 rotation);
		// void setTexture(std::string name);
		// void setTexture(Texture* texture);
		// void bindTexture();
		// void unBindTexture();
		Buffer* getBuffer();
		void cleanUp();
		//bool operator <= (const FaceMesh& mesh);
		// void operator() (FaceMesh* mesh, glm::vec3 pos);
		// void operator() (Buffer*, Texture*, glm::vec3 pos);
		/*bool isCombo();
		Buffer* getCombo(int index);*/
		std::vector<GLushort> createIncides();
		// Physics::BoxCollider& getCollider();
	private:
		void setupBufferStructure(GLboolean is3D = GL_TRUE);
	};
	// FaceMesh& operator+=(FaceMesh& mesh1, const FaceMesh& mesh2);

	struct BlockMesh {
		std::vector<FaceMesh*> faces;
		glm::vec3 position;
		Buffer* buffer;
		Texture* texture;
		BlockMesh();
		void addFace(FaceMesh* face, glm::vec3 pos = glm::vec3());
		void setPosition(glm::vec3 position);

		void setTexture(std::string name);
		void setTexture(Texture* texture);
		void bindTexture();
	};
};




