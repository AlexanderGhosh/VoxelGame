#include "Mesh.h"
namespace Mesh {
	FaceMesh::FaceMesh(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC) {
		vertices = vert;
		normals = norm;
		texCoords = texC;
		buffer = Buffer();
		setupBufferStructure();
	}
	void FaceMesh::setupBufferStructure(GLboolean is3D) {
		std::vector<GLfloat> vertices_;
		for (int i = 0; i < 6; i++)
		{
			vertices_.push_back(vertices[i].x + position.x);
			vertices_.push_back(vertices[i].y + position.y);
			vertices_.push_back(vertices[i].z + position.z);

			vertices_.push_back(normals[i].x + position.x);
			vertices_.push_back(normals[i].y + position.y);
			vertices_.push_back(normals[i].z + position.z);

			vertices_.push_back(texCoords[i].x + position.x);
			vertices_.push_back(texCoords[i].y + position.y);
			if (is3D) {
				vertices_.push_back(texCoords[i].z + position.z);
			}
		}
		buffer.setBufferData(vertices_);
		buffer.setStructure({ (is3D) ? (GLuint)9 : (GLuint)8, 3, {3, 3, (is3D) ? (GLuint)3 : (GLuint)2} });
	}
	void FaceMesh::bindTexture() {
		// texture.bind();
	}
	void FaceMesh::setPosition(glm::vec3 position) {
		this->position = position;
	}
	void FaceMesh::setTexture(std::string name) {
		// texture = Texture(name, GL_FALSE);
	}
	void FaceMesh::unBindTexture() {
		// texture.unBind();
	}
	void FaceMesh::setRotation(glm::vec3 rotation) {
		this->rotation = rotation;
	}

	void BlockMesh::addFace(FaceMesh& face, glm::vec3 pos) {
		if (faces.size() > 6) return;
		face.setPosition(pos);
		faces.push_back(face);
		buffer.merge(face.buffer);
	}
	void BlockMesh::setPosition(glm::vec3 position) {
		this->position = position;
	}
	void BlockMesh::setTexture(std::string name) {
		// texture = Texture(name, GL_FALSE);
	}
	void BlockMesh::bindTexture() {
		// texture.bind();
	}
};