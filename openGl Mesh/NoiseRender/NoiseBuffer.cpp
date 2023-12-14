#include "NoiseBuffer.h"
#include <glad/glad.h>

NoiseBuffer::NoiseBuffer() : VAO(), VBO(), size_()
{
}

NoiseBuffer::NoiseBuffer(float* data, unsigned int size)
{
	size_ = size;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0); // vert pos

	unBind();
}

NoiseBuffer::~NoiseBuffer()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	VAO = VBO = 0;
}

NoiseBuffer::NoiseBuffer(NoiseBuffer&& other) noexcept
{
	VBO = other.VBO;
	VAO = other.VAO;
	size_ = other.size_;

	other.VAO = other.VBO = 0;
}

NoiseBuffer& NoiseBuffer::operator=(NoiseBuffer&& other) noexcept
{
	VBO = other.VBO;
	VAO = other.VAO;
	size_ = other.size_;

	other.VAO = other.VBO = 0;
	return *this;
}

size_t NoiseBuffer::size() const
{
	return size_;
}

void NoiseBuffer::bind() const
{
	glBindVertexArray(VAO);
}

void NoiseBuffer::unBind() const
{
	glBindVertexArray(0);
}
