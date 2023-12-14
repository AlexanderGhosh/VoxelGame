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

BufferGeom::BufferGeom(BufferGeom&& other) noexcept : BufferGeom()
{
	VBO = other.VBO;
	VAO = other.VAO;
	size_ = other.size_;

	other.VBO = other.VAO = 0;
	other.size_ = 0;
}

BufferGeom& BufferGeom::operator=(BufferGeom&& other) noexcept
{
	VBO = other.VBO;
	VAO = other.VAO;
	size_ = other.size_;

	other.VBO = other.VAO = 0;
	other.size_ = 0;
	return *this;
}

void BufferGeom::setUp(const GeomData* data, unsigned int size)
{
	cleanUp();
	glGenVertexArrays(1, &VAO); // VAO
	glGenBuffers(1, &VBO); // VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GeomData), data, GL_STATIC_DRAW);

	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GeomData), 0);

	glEnableVertexAttribArray(0);

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
