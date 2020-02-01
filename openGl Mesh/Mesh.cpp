#include "Mesh.h"
namespace Mesh {
	FaceMesh::FaceMesh(std::vector<glm::vec3>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC) {
		vertices = vert;
		normals = norm;
		texCoords = texC;
		buffer = new Buffer();
		position = { 0, 0, 0 };
		setupBufferStructure();
	}
	FaceMesh::FaceMesh(GLboolean loadTex) {
		vertices = std::vector<glm::vec3>();
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
		//std::vector<GLushort> indices = createIncides();
		std::vector<GLfloat> bufferData;
		for (int i = 0; i < 6; i++)
		{
			bufferData.push_back(vertices[i].x + position.x);
			bufferData.push_back(vertices[i].y + position.y);
			bufferData.push_back(vertices[i].z + position.z);

			bufferData.push_back(normals[i].x + position.x);
			bufferData.push_back(normals[i].y + position.y);
			bufferData.push_back(normals[i].z + position.z);

			bufferData.push_back(texCoords[i].s + position.x);
			bufferData.push_back(texCoords[i].t + position.y);
			if (is3D) {
				bufferData.push_back(texCoords[i].p + position.z);
			}
		}
		buffer->setBufferData(bufferData);
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
		vertices = std::vector<glm::vec3>();
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
	void FaceMesh::operator() (FaceMesh* mesh, glm::vec3 pos) {
		mesh->setPosition(pos);
		*this += *mesh;
	}
	void FaceMesh::operator() (Buffer* buffer, Texture* texture, glm::vec3 pos) {
		this->buffer->merge(*buffer);
		comboOf.push_back(buffer);
		setTexture(texture);
		setPosition(pos);
		setRotation({ 0, 0, 0 });
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
	std::vector<GLushort> FaceMesh::createIncides() {
		std::vector<GLushort> indices;
		std::vector<glm::vec3> vertices_unique;
		for (auto& vertex : vertices) {
			std::vector<glm::vec3>::iterator foundAt = std::find(vertices_unique.begin(), vertices_unique.end(), vertex);
			if (foundAt == vertices_unique.end()) {
				vertices_unique.push_back(vertex);
				indices.push_back(vertices_unique.size() - 1);
			}
			else {
				indices.push_back(std::distance(vertices_unique.begin(), foundAt));
			}
		}
		vertices = vertices_unique;
		return indices;
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