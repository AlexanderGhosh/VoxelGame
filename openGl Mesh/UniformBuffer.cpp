#include "UniformBuffer.h"
#include <glad/glad.h>
#include <glm.hpp>
#include "Helpers/Functions.h"

int UniformBuffer::maxBingingPoints = -1;

UniformBuffer::UniformBuffer() : UBO(0), _bindingPoint(-1)
{
	if (maxBingingPoints < 0)
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBingingPoints);
}

UniformBuffer::~UniformBuffer()
{
	if (UBO == 0) return;
	glDeleteBuffers(1, &UBO);
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept : UniformBuffer()
{
	UBO = other.UBO;
	_bindingPoint = other._bindingPoint;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{
	UBO = other.UBO;
	_bindingPoint = other._bindingPoint;
	return *this;
}

const void UniformBuffer::init(unsigned dataSize, short bindingPoint)
{
	_bindingPoint = bindingPoint;
	glGenBuffers(1, &UBO);
	bind();

	glBufferData(GL_UNIFORM_BUFFER, dataSize, nullptr, GL_DYNAMIC_DRAW);

	unBind();

	glBindBufferRange(GL_UNIFORM_BUFFER, _bindingPoint, UBO, 0, dataSize);
}

void UniformBuffer::allocate(unsigned int bytes, unsigned int bindingPoint)
{
	glGenBuffers(1, &UBO);
	bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, UBO, 0, bytes);

	unBind();
}


void UniformBuffer::fill(unsigned int offset, unsigned int size, const void* data) const
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	unBind();
}

void UniformBuffer::bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
}

void UniformBuffer::unBind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}