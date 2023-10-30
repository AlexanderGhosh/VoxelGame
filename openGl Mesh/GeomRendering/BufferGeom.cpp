#include "BufferGeom.h"
// #define GLEW_STATIC
#include <glad/glad.h>

#include "GeomData.h"

BufferGeom::BufferGeom() : VBO(0), VAO(0), size_(0)
{
}

BufferGeom::~BufferGeom()
{
	if (!VAO || !VBO) {
		cleanUp();
	}
}

void BufferGeom::setUp(const GeomData* data, unsigned int size)
{
	glGenBuffers(1, &VBO); // VBO
	glGenVertexArrays(1, &VAO); // VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GeomData), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GeomData), 0);

	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(GeomData), (void*)offsetof(GeomData, cubeType_));

	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(GeomData), (void*)offsetof(GeomData, textureIndex_));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	this->size_ = size;
}

void BufferGeom::bind() const
{
	glBindVertexArray(VAO);
}

void BufferGeom::unbind() const
{
	glBindVertexArray(0);
}

unsigned int BufferGeom::size() const
{
	return size_;
}

void BufferGeom::cleanUp()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	VAO = VBO = 0;
}
