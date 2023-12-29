#pragma once

namespace GUI {
	namespace Utils {
		class Vertex;
	}
}

class GUIBuffer {
public:
	GUIBuffer();
	~GUIBuffer();

	GUIBuffer(const GUIBuffer&);
	GUIBuffer& operator=(const GUIBuffer&);

	GUIBuffer(GUIBuffer&& other) noexcept;
	GUIBuffer& operator=(GUIBuffer&& other) noexcept;

	void setUp(GUI::Utils::Vertex* data, unsigned int size);

	void bind();
	void unBind();

	inline void empty() { _size = VBO = VAO = 0; }

	inline size_t size() const { return _size; }
private:
	unsigned int VBO, VAO;
	size_t _size;
};