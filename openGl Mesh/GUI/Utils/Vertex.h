#pragma once
#include "Float2.h"
#include "Float4.h"

namespace GUI {
	namespace Utils{
		class Vertex {
		public:
			Vertex() = default;
			Vertex(Float2 pos, Float4 col) : position(pos), colour(col) { }
			Float2 position;
			Float4 colour;
		};
	}
}