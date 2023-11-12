#pragma once
#include "OBJLoader.h"

class IndexedBuffer
{
private:
	unsigned int VAO, VBO, EBO;
	size_t _size;
public:
	IndexedBuffer();
	~IndexedBuffer();
	void setUp(const objl::Mesh& mesh);

	size_t size() const;

	void bind() const;
	void unbind() const;

	void cleanUp();
};

