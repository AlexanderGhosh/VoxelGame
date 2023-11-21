#include "UniformBuffer.h"
#include <glad/glad.h>
#include <glm.hpp>
#include "Helpers/Functions.h"

int UniformBuffer::maxBingingPoints = -1;

UniformBuffer::UniformBuffer() : UBO(0), bindingPoint(-1), positions()
{
	if (maxBingingPoints < 0)
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBingingPoints);
}

UniformBuffer::UniformBuffer(const std::string& types, char bindingPoint) : UniformBuffer(split(types, ", "), bindingPoint)
{
}

UniformBuffer::UniformBuffer(const std::vector<std::string>& types, char bindingPoint) : UniformBuffer() {
	unsigned dataSize = 0, offset = 0;
	for (const std::string& type : types) {
		// floats
		if (type == "f") {
			dataSize = sizeof(float);
		}
		else if (type == "f2") {
			dataSize = sizeof(glm::vec2);
		}
		else if (type == "f3") {
			dataSize = sizeof(glm::vec3);
		}
		else if (type == "f4") {
			dataSize = sizeof(glm::vec4);
		}
		// ints
		else if (type == "i") {
			dataSize = sizeof(int);
		}
		else if (type == "i2") {
			dataSize = sizeof(glm::ivec2);
		}
		else if (type == "i3") {
			dataSize = sizeof(glm::ivec3);
		}
		else if (type == "i4") {
			dataSize = sizeof(glm::ivec4);
		}
		// matrices
		else if (type == "m2") {
			dataSize = sizeof(glm::mat2);
		}
		else if (type == "m3") {
			dataSize = sizeof(glm::mat3);
		}
		else if (type == "m4") {
			dataSize = sizeof(glm::mat4);
		}
		else if (contains(type, "m4")) {
			std::string num = replaceAll(type, "m4", "");
			int a = std::stoi(num);
			dataSize = sizeof(glm::mat4) * a;
		}
		positions.push_back({ offset, dataSize });
		offset += dataSize;
	}
	init(offset, bindingPoint);
}

const void UniformBuffer::init(unsigned dataSize, short bindingPoint)
{
	this->bindingPoint = bindingPoint;
	glGenBuffers(1, &UBO);
	bind();

	glBufferData(GL_UNIFORM_BUFFER, dataSize, nullptr, GL_DYNAMIC_DRAW);

	unBind();

	glBindBufferRange(GL_UNIFORM_BUFFER, this->bindingPoint, UBO, 0, dataSize);
}

void UniformBuffer::cleanUp()
{
	bind();
	glDeleteBuffers(1, &UBO);
	unBind();
	positions.clear();
}

const short UniformBuffer::getBindingPoint() const
{
	return bindingPoint;
}

void UniformBuffer::allocate(unsigned int bytes, unsigned int bindingPoint)
{
	glGenBuffers(1, &UBO);
	bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, UBO, 0, bytes);

	unBind();
}


void UniformBuffer::fill(unsigned offset, unsigned size, const void* data) const
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	unBind();
}

void UniformBuffer::fill(unsigned position, const void* data) const
{
	const std::pair<unsigned, unsigned>& pair = positions[position];
	fill(pair.first, pair.second, data);
}

void UniformBuffer::bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
}

void UniformBuffer::unBind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}