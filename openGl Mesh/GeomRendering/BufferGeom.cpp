#include "BufferGeom.h"

#include <glad/glad.h>

#include "GeomData.h"

BufferGeom::BufferGeom() : VBO(0), VAO(0), size_(0)
{
}

BufferGeom::~BufferGeom()
{
	cleanUp();
}

void BufferGeom::setUp(const GeomData* data, unsigned int size)
{
	cleanUp();
	glGenBuffers(1, &VBO); // VBO
	glGenVertexArrays(1, &VAO); // VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GeomData), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GeomData), 0);

	glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(GeomData), (void*)offsetof(GeomData, cubeType_));

	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(GeomData), (void*)offsetof(GeomData, textureIndex_));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	this->size_ = size;
}

void BufferGeom::realloc(const GeomData* data, unsigned int size)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GeomData), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	if(!(VAO || VBO)) return;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	VAO = VBO = 0;
}
