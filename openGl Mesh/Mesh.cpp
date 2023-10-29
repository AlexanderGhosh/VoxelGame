#include "Mesh.h"
#include "Buffer.h"
#include "Textures/Texture.h"

namespace Mesh {
	FaceMesh::FaceMesh() : vertices(), normal(), texCoords(), position(), buffer(), texture(), comboOf(), rotation() { }

	FaceMesh::FaceMesh(std::vector<glm::vec3>vert, glm::vec3 norm, std::array<glm::vec3, 6 > texC) {
		vertices = vert;
		normal = norm;
		texCoords = texC;
		buffer = new Buffer();
		position = { 0, 0, 0 };
		setupBufferStructure();
	}
	void FaceMesh::setupBufferStructure(bool is3D) {
		//std::vector<GLushort> indices = createIncides();
		std::vector<float> bufferData;
		for (int i = 0; i < 6; i++)
		{
			bufferData.push_back(vertices[i].x + position.x);
			bufferData.push_back(vertices[i].y + position.y);
			bufferData.push_back(vertices[i].z + position.z);

			bufferData.push_back(normal.x);
			bufferData.push_back(normal.y);
			bufferData.push_back(normal.z);

			bufferData.push_back(texCoords[i].s);
			bufferData.push_back(texCoords[i].t);
			if (is3D) {
				bufferData.push_back(texCoords[i].p + position.z);
			}
		}
		buffer->setBufferData(bufferData);
		buffer->setStructure({ (is3D) ? (unsigned int)9 : (unsigned int)8, 3, {3, 3, (is3D) ? (unsigned int)3 : (unsigned int)2} });
	}
	/*void FaceMesh::bindTexture() {
		texture->bind();
	}*/
	void FaceMesh::setPosition(glm::vec3 position) {
		this->position = position;
	}
	///*void FaceMesh::setTexture(std::string name) {
	//	texture = new Texture(name, GL_FALSE);
	//}*/
	//void FaceMesh::setTexture(Texture* texture) {
	//	this->texture = texture;
	//}
	///*void FaceMesh::unBindTexture() {
	//	texture->unBind();
	//}*/
	//void FaceMesh::setRotation(glm::vec3 rotation) {
	//	this->rotation = rotation;
	//}
	Buffer* FaceMesh::getBuffer() {
		return buffer;
	}
	void FaceMesh::cleanUp() {
		vertices = std::vector<glm::vec3>();
		texCoords = std::array<glm::vec3, 6>();
		position = { 0, 0, 0 };
		rotation = { 0, 0, 0 };
		buffer->destroy();
		texture->unBind();
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


	BlockMesh::BlockMesh() : faces(), position(), buffer(), texture() {
		faces = std::vector<FaceMesh*>();
		position = { 0, 0, 0 };
		buffer = new Buffer();
		texture = new Texture("grass", GL_FALSE);
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