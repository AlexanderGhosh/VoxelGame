#pragma once
#include <vector>
#include <string>

class UniformBuffer {
private:
	unsigned UBO;
	int _bindingPoint;
	// statics
	static int maxBingingPoints;

	const void init(unsigned dataSize, short bindingPoint = -1);
public:
	UniformBuffer();
	~UniformBuffer();

	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

	UniformBuffer(UniformBuffer&& other) noexcept;
	UniformBuffer& operator=(UniformBuffer&& other) noexcept;


	void allocate(unsigned int bytes, unsigned int bindingPoint);

	void fill(unsigned int offset, unsigned int size, const void* data) const;

	void bind() const;
	void unBind() const;
};