#pragma once
#include <vector>
#include "Vertex.h"

namespace GUI {
	namespace Utils {
		class Vertex;
		// in screen space (clip space)
		using DrawData = std::vector<Vertex>;
	}
}