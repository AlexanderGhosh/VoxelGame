#pragma once

struct GreedyData;
class BufferGreedy
{
public:
	BufferGreedy();
	~BufferGreedy();

	BufferGreedy(BufferGreedy&& other) noexcept;
	BufferGreedy& operator=(BufferGreedy&& other) noexcept;

	BufferGreedy(const BufferGreedy&) = delete;
	BufferGreedy& operator=(const BufferGreedy&) = delete;

	void setUp(const GreedyData* data, unsigned int size);
	void realloc(const GreedyData* data, unsigned int size);

	void bind() const;
	void unbind() const;

	unsigned int size() const;

	void cleanUp();
private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int size_;
};

