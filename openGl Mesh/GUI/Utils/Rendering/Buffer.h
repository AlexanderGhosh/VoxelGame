#pragma once

namespace GUI {
	namespace Utils {
		class Vertex;

		class Buffer {
		public:
			Buffer();
			~Buffer();

			Buffer(const Buffer&) = delete;
			Buffer& operator=(const Buffer&) = delete;

			Buffer(Buffer&& other) noexcept;
			Buffer& operator=(Buffer&& other) noexcept;

			void setUp(const GUI::Utils::Vertex* data, unsigned int size);
			void realoc(const GUI::Utils::Vertex* data, unsigned int size) const;

			void bind() const;
			void unBind() const;

			inline void empty() { _size = VBO = VAO = 0; }
			inline bool valid() const { return VAO && VBO; }

			inline size_t size() const { return _size; }
		private:
			unsigned int VBO, VAO;
			size_t _size;
		};
	}
}