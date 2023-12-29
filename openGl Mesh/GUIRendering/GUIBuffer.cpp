#include "GUIBuffer.h"
#include <glad/glad.h>
#include "../GUI/Utils/Vertex.h"
#include <cstddef> //offsetof

GUIBuffer::GUIBuffer() : VBO(), VAO(), _size(0)
{
}

GUIBuffer::~GUIBuffer()
{
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    VBO = VAO = 0;
}

GUIBuffer::GUIBuffer(const GUIBuffer& other) : GUIBuffer()
{
    VBO = other.VBO;
    VAO = other.VAO;
    _size = other._size;
}

GUIBuffer& GUIBuffer::operator=(const GUIBuffer& other)
{
    VBO = other.VBO;
    VAO = other.VAO;
    _size = other._size;
    return *this;
}

GUIBuffer::GUIBuffer(GUIBuffer&& other) noexcept : GUIBuffer()
{
    VBO = other.VBO;
    VAO = other.VAO;
    _size = other._size;

    other.empty();
}

GUIBuffer& GUIBuffer::operator=(GUIBuffer&& other) noexcept
{
    VBO = other.VBO;
    VAO = other.VAO;
    _size = other._size;

    other.empty();
    return *this;
}

void GUIBuffer::setUp(GUI::Utils::Vertex* data, unsigned int size)
{
    _size = size;
    glGenBuffers(1, &VBO); // VBO
    glGenVertexArrays(1, &VAO); // VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(GUI::Utils::Vertex), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GUI::Utils::Vertex), (void*)offsetof(GUI::Utils::Vertex, position));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GUI::Utils::Vertex), (void*)offsetof(GUI::Utils::Vertex, colour));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void GUIBuffer::bind()
{
    glBindVertexArray(VAO);
}

void GUIBuffer::unBind()
{
    glBindVertexArray(0);
}
