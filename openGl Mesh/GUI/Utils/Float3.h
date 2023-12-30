#pragma once

namespace GUI {
	namespace Utils {
		class Float3 {
		public:
			float r, g, b;
			Float3() : r(0), g(0), b(0) { }
			Float3(float r, float g, float b) : r(r), g(g), b(b) { }
		};
	}
}