#include "Mesh.h"

FaceMesh::FaceMesh(std::array<glm::vec3, 6>vert, std::array<glm::vec3, 6 > norm, std::array<glm::vec3, 6 > texC, GLuint lightLv, Structure structure) {
	vertices = vert;
	normals = norm;
	texCoords = texC;
	lightLevel = lightLv;
    data_s = structure;
}

void Mesh::getVertices(GLboolean vec3) {
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
            if (vec3) {
                vertices.push_back(face.texCoords[i].z);
            }
		}
	}
    data_s.data = vertices;
}

GLboolean BlockMesh::addFace(FaceMesh& face, GLboolean force) {
    if (faces.size() < 1) force = GL_TRUE;
	if (faces.size() >= 6) return GL_FALSE;
    try {
        if (data_s != face.data_s) {
            data_s = face.data_s;
            if (!force) {
                return GL_FALSE;
            }
        }
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }

	try {
		faces.push_back(face);
		return GL_TRUE;
	}
	catch (std::exception e) {
		return GL_FALSE;
	}
}