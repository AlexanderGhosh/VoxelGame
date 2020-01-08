#include "Mesh.h"

MeshFace::MeshFace(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::ivec2, 6 > texC, GLuint lightLv) {
	vertices = vert;
	normals = norm;
	texCoords = texC;
	lightLevel = lightLv;
}

std::vector<GLfloat> BlockMesh::getVertices() {
	std::vector<GLfloat> vertices;
	for (auto& face : faces) {
		for (int i = 0; i < 6; i++)
		{
			vertices.push_back(face.vertices[i].x);
			vertices.push_back(face.vertices[i].y);
			vertices.push_back(face.vertices[i].z);

			vertices.push_back(face.normals[i].x);
			vertices.push_back(face.normals[i].y);
			vertices.push_back(face.normals[i].z);

			vertices.push_back(face.texCoords[i].x);
			vertices.push_back(face.texCoords[i].y);
		}
	}
	return vertices;
}

GLboolean BlockMesh::addFace(MeshFace& face) {
	if (faces.size() >= 6) return GL_FALSE;
	try {
		faces.push_back(face);
		return GL_TRUE;
	}
	catch (std::exception e) {
		return GL_FALSE;
	}
}