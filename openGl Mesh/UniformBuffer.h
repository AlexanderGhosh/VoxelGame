#pragma 
#include <vector>
#include <string>

class UniformBuffer {
private:
	unsigned UBO;
	int bindingPoint;
	std::vector<std::pair<unsigned, unsigned>> positions;
	// statics
	static int maxBingingPoints;

	const void init(unsigned dataSize, short bindingPoint = -1);
public:
	UniformBuffer();
	UniformBuffer(const std::string& types, char bindingPoint = -1);
	UniformBuffer(const std::vector<std::string>& types, char bindingPoint);
	~UniformBuffer() = default;

	const short getBindingPoint() const;

	void allocate(unsigned int bytes, unsigned int bindingPoint);

	void fill(unsigned offset, unsigned size, const void* data) const;
	void fill(unsigned position, const void* data) const;

	void bind() const;
	void unBind() const;

	void cleanUp();
};