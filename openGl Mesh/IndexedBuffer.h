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

	IndexedBuffer(const IndexedBuffer&) = delete;
	IndexedBuffer& operator=(const IndexedBuffer&) = delete;

	IndexedBuffer(IndexedBuffer&& other) noexcept;
	IndexedBuffer& operator=(IndexedBuffer&& other) noexcept;

	void setUp(const Mesh& mesh);

	inline size_t size() const { return _size; }

	void bind() const;
	void unbind() const;
};

