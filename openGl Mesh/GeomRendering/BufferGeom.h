#pragma once

struct GeomData;
class BufferGeom
{
public:
	BufferGeom();
	~BufferGeom();

	BufferGeom(const BufferGeom&) = delete;
	BufferGeom& operator=(const BufferGeom&) = delete;

	BufferGeom(BufferGeom&& other) noexcept;
	BufferGeom& operator=(BufferGeom&& other) noexcept;

	void setUp(const GeomData* data, unsigned int size);
	void realloc(const GeomData* data, unsigned int size);

	void bind() const;
	void unbind() const;

	unsigned int size() const;

	void cleanUp();
private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int size_;
};

