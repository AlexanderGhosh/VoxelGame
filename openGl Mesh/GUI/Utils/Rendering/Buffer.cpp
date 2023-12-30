#include "Buffer.h"
#include <glad/glad.h>
#include "../Vertex.h"
#include <cstddef> //offsetof

using namespace GUI::Utils;

Buffer::Buffer() : VBO(), VAO(), _size(0)
{
}

Buffer::~Buffer()
{
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    VBO = VAO = 0;
}

Buffer::Buffer(Buffer&& other) noexcept : Buffer()
{
    VBO = other.VBO;
    VAO = other.VAO;
    _size = other._size;

    other.empty();
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    VBO = other.VBO;
    VAO = other.VAO;
    _size = other._size;

    other.empty();
    return *this;
}

void Buffer::setUp(const GUI::Utils::Vertex* data, unsigned int size)
{
    if (valid()) throw "Cannot call setUp on a valid buffer (GUI)";
    _size = size;
    glGenBuffers(1, &VBO); // VBO
    glGenVertexArrays(1, &VAO); // VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(GUI::Utils::Vertex), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GUI::Utils::Vertex), (void*)offsetof(GUI::Utils::Vertex, position));

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void GUI::Utils::Buffer::realoc(const GUI::Utils::Vertex* data, unsigned int size) const
{
    if (!valid()) throw "Cannot call realoc on an invalid buffer (GUI)";
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (_size < size) {
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(GUI::Utils::Vertex), data, GL_STATIC_DRAW);
        const_cast<unsigned int&>(_size) = size;
    }
    else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(GUI::Utils::Vertex), data);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffer::bind() const
{
    glBindVertexArray(VAO);
}

void Buffer::unBind() const
{
    glBindVertexArray(0);
}
