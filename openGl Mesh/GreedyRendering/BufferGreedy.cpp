#include "BufferGreedy.h"

#include <glad/glad.h>

#include "GreedyData.h"

BufferGreedy::BufferGreedy() : VBO(0), VAO(0), size_(0)
{
}

BufferGreedy::~BufferGreedy()
{
	cleanUp();
}

void BufferGreedy::setUp(const GreedyData* data, unsigned int size)
{
	cleanUp();
	glGenBuffers(1, &VBO); // VBO
	glGenVertexArrays(1, &VAO); // VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GreedyData), data, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GreedyData), (void*) offsetof(GreedyData, _corner0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GreedyData), (void*) offsetof(GreedyData, _corner1));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GreedyData), (void*) offsetof(GreedyData, _corner2));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GreedyData), (void*) offsetof(GreedyData, _corner3));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GreedyData), (void*)offsetof(GreedyData, _normal));

	glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(GreedyData), (void*) offsetof(GreedyData, _materialIdx));


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	glBindVertexArray(0);

	this->size_ = size;
}

void BufferGreedy::realloc(const GreedyData* data, unsigned int size)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(GreedyData), data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	this->size_ = size;
}

void BufferGreedy::bind() const
{
	glBindVertexArray(VAO);
}

void BufferGreedy::unbind() const
{
	glBindVertexArray(0);
}

unsigned int BufferGreedy::size() const
{
	return size_;
}

void BufferGreedy::cleanUp()
{
	if(!(VAO || VBO)) return;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	VAO = VBO = 0;
}
