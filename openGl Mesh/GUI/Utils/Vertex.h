#pragma once
#include "Float2.h"
#include "Float4.h"

namespace GUI {
	namespace Utils{
		class Vertex {
		public:
			Vertex() = default;
			Vertex(Float2 pos) : position(pos) { }
			Float2 position;
		};
	}
}