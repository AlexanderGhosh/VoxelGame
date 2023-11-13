#pragma once

class Mesh;
class IndexedBuffer
{
private:
	unsigned int VAO, VBO, EBO;
	size_t _size;
public:
	IndexedBuffer();
	IndexedBuffer(const Mesh& mesh);
	~IndexedBuffer();
	void setUp(const Mesh& mesh);

	size_t size() const;

	void bind() const;
	void unbind() const;

	void cleanUp();
};

