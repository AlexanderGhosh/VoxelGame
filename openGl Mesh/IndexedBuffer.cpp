#include "IndexedBuffer.h"
#include <glad/glad.h>
#include <glm.hpp>
#include "Helpers/ModelLoaders/Mesh.h"

IndexedBuffer::IndexedBuffer() : VBO(0), VAO(0), EBO(0), _size(0)
{
}
IndexedBuffer::IndexedBuffer(const Mesh& mesh) {
	setUp(mesh);
}

IndexedBuffer::~IndexedBuffer()
{
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		unsigned int buffers[2] = { VBO, EBO };
		glDeleteBuffers(2, buffers);
		VBO = VAO = EBO = 0;
	}
}

IndexedBuffer::IndexedBuffer(IndexedBuffer&& other) noexcept
{
	VAO = other.VAO;
	VBO = other.VBO;
	EBO = other.EBO;
	_size = other._size;
}

IndexedBuffer& IndexedBuffer::operator=(IndexedBuffer&& other) noexcept
{
	VAO = other.VAO;
	VBO = other.VBO;
	EBO = other.EBO;
	_size = other._size;
	return *this;
}

void IndexedBuffer::setUp(const Mesh& mesh)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	unsigned int sizeOfData = 32 * mesh.vertexPoss.size(); // simplified equation for the sum of sizes of pos, norm, tex in bytes
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeOfData, nullptr , GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.vertexPoss.size() * sizeof(glm::vec3), &mesh.vertexPoss[0]);
	glBufferSubData(GL_ARRAY_BUFFER, mesh.vertexPoss.size() * sizeof(glm::vec3), mesh.vertexNorms.size() * sizeof(glm::vec3), &mesh.vertexNorms[0]);
	glBufferSubData(GL_ARRAY_BUFFER, mesh.vertexPoss.size() * sizeof(glm::vec3) + mesh.vertexNorms.size() * sizeof(glm::vec3), mesh.vertexTexCoords.size() * sizeof(glm::vec2), &mesh.vertexTexCoords[0]);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0); // vert pos

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(mesh.vertexPoss.size() * sizeof(glm::vec3))); // vert norm

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(mesh.vertexPoss.size() * sizeof(glm::vec3) + mesh.vertexNorms.size() * sizeof(glm::vec3))); // vert tex coords


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	_size = mesh.indices.size();
}

void IndexedBuffer::bind() const
{
	glBindVertexArray(VAO);
}

void IndexedBuffer::unbind() const
{
	glBindVertexArray(0);
}
