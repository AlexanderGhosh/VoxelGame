#pragma once
class NoiseBuffer
{
public:
	NoiseBuffer();
	NoiseBuffer(float* data, unsigned int size);

	~NoiseBuffer();

	NoiseBuffer(NoiseBuffer& other) = delete;
	NoiseBuffer operator=(NoiseBuffer& other) = delete;

	NoiseBuffer(NoiseBuffer&& other) noexcept;
	NoiseBuffer& operator=(NoiseBuffer&& other) noexcept;

	size_t size() const;

	void bind() const;
	void unBind() const;
private:
	unsigned int VAO, VBO;
	size_t size_;
};

