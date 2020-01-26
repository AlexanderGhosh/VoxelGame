#include "Mesh.h"
namespace Mesh {
	FaceMesh::FaceMesh(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC) {
		vertices = vert;
		normals = norm;
		texCoords = texC;
		buffer = new Buffer();
		position = { 0, 0, 0 };
		setupBufferStructure();
	}
	FaceMesh::FaceMesh(GLboolean loadTex) {
		vertices = std::array<glm::vec3, 6>();
		normals = std::array<glm::vec3, 6>();
		texCoords = std::array<glm::vec3, 6>();
		position = glm::vec3(3, 2, 0);
		buffer = new Buffer();
		rotation = { 0, 0, 0 };
		if (loadTex) {
			texture = new Texture(false);
		}
		else {
			texture = new Texture("grass", false);
		}
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
		buffer->setBufferData(vertices_);
		buffer->setStructure({ (is3D) ? (GLuint)9 : (GLuint)8, 3, {3, 3, (is3D) ? (GLuint)3 : (GLuint)2} });
	}
	void FaceMesh::bindTexture() {
		texture->bind();
	}
	void FaceMesh::setPosition(glm::vec3 position) {
		this->position = position;
	}
	void FaceMesh::setTexture(std::string name) {
		texture = new Texture(name, GL_FALSE);
	}
	void FaceMesh::setTexture(Texture* texture) {
		this->texture = texture;
	}
	void FaceMesh::unBindTexture() {
		texture->unBind();
	}
	void FaceMesh::setRotation(glm::vec3 rotation) {
		this->rotation = rotation;
	}
	Buffer* FaceMesh::getBuffer() {
		return buffer;
	}
	void FaceMesh::cleanUp() {
		vertices = std::array<glm::vec3, 6>();
		normals = std::array<glm::vec3, 6>();
		texCoords = std::array<glm::vec3, 6>();
		position = { 0, 0, 0 };
		rotation = { 0, 0, 0 };
		buffer->destroy();
		texture->unBind();
	}
	FaceMesh& operator+= (FaceMesh& mesh1, const FaceMesh& mesh2) {
		mesh1.buffer->merge(*mesh2.buffer);
		mesh1.comboOf.push_back(mesh2.buffer);
		mesh1.setTexture(mesh2.texture);
		mesh1.setPosition(mesh2.position);
		mesh1.setRotation(mesh2.rotation);
		return mesh1;
	}
	bool FaceMesh::operator <= (const FaceMesh& mesh) {
		return buffer <= mesh.buffer;
	}
	bool FaceMesh::isCombo() {
		if (comboOf.size() > 1) {
			return true;
		}
		else {
			return false;
		}
	}
	Buffer* FaceMesh::getCombo(int index) {
		if (index < 0 || index + 1 > comboOf.size()) return nullptr;
		return comboOf[index];
	}

	void BlockMesh::addFace(FaceMesh* face, glm::vec3 pos) {
		if (faces.size() > 6) return;
		// face.setPosition(pos);
		faces.push_back(face);
		faces.back()->setPosition(pos);
		// buffer->merge(*face.buffer);
	}
	void BlockMesh::setPosition(glm::vec3 position) {
		this->position = position;
	}
	void BlockMesh::setTexture(std::string name) {
		texture = new Texture(name, GL_FALSE);
	}
	void BlockMesh::bindTexture() {
		texture->bind();
	}
	void BlockMesh::setTexture(Texture* texture) {
		this->texture = texture;
	}
};