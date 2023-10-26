#pragma once

struct GeomData;
class BufferGeom
{
public:
	BufferGeom();

	void setUp(const GeomData* data, unsigned int size);

	void bind() const;
	void unbind() const;

	unsigned int size() const;

	void cleanUp();
private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int size_;
};

