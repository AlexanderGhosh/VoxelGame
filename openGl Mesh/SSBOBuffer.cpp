#include "SSBOBuffer.h"
#include "glad/glad.h"
#include <vcruntime_string.h>

SSBOBuffer::SSBOBuffer() : ssbo_(0), bindingPoint_(0)
{
}

SSBOBuffer::~SSBOBuffer()
{
	cleanUp();
}

void SSBOBuffer::bind() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint_, ssbo_);
}

void SSBOBuffer::unBind() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBOBuffer::cleanUp()
{
	if (ssbo_) {
		glDeleteBuffers(1, &ssbo_);
		ssbo_ = 0;
	}
}

void SSBOBuffer::setUp(void* data, unsigned int size)
{
	glGenBuffers(1, &ssbo_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_STATIC_DRAW);
	void* buffer = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT); // maybe | GL_MAP_INVALIDATE_BUFFER_BIT
	memcpy(buffer, data, size);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void SSBOBuffer::setBindingPoint(const unsigned int point)
{
	bindingPoint_ = point;
}
