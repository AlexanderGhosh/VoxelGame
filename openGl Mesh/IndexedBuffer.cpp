#include "IndexedBuffer.h"
#include <glad/glad.h>

IndexedBuffer::IndexedBuffer() : VBO(0), VAO(0), EBO(0), _size(0)
{
}

IndexedBuffer::~IndexedBuffer()
{
	cleanUp();
}

void IndexedBuffer::setUp(const objl::Mesh& mesh)
{
	glCreateVertexArrays(1, &VAO);
	unsigned int buffers[2] = { VBO, EBO };
	glCreateBuffers(2, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), mesh.Vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)0); // vert pos

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, Normal)); // vert norm

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, TextureCoordinate)); // vert tex coords


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), mesh.Indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	_size = mesh.Indices.size();
}

size_t IndexedBuffer::size() const
{
	return _size;
}

void IndexedBuffer::bind() const
{
	glBindVertexArray(VAO);
}

void IndexedBuffer::unbind() const
{
	glBindVertexArray(0);
}

void IndexedBuffer::cleanUp()
{
	if (VAO == 0) {
		glDeleteVertexArrays(1, &VAO);
		unsigned int buffers[2] = { VBO, EBO };
		glDeleteBuffers(2, buffers);
		VBO = VAO = EBO = 0;
	}
}
