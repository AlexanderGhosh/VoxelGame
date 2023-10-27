#include "BufferGeom.h"
#include <gl/glew.h>

#include "GeomData.h"

BufferGeom::BufferGeom() : VBO(0), VAO(0), size_(0)
{
}

void BufferGeom::setUp(const GeomData* data, unsigned int size)
{
	glGenBuffers(1, &VBO); // VBO
	glGenVertexArrays(1, &VAO); // VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GeomData), data, GL_STATIC_DRAW);


	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GeomData), 0);
	glEnableVertexAttribArray(0);

	// glVertexAttribPointer(1, 1, GL_INT, GL_TRUE, sizeof(GeomData), (void*)(3*sizeof(float)));
	// glEnableVertexAttribArray(1);

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
}
