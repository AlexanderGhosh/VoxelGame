#pragma once

namespace GUI {
	namespace Utils {
		class Float4 {
		public:
			float r, g, b, a;
			Float4() : r(0), g(0), b(0), a(0) { }
			Float4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
		};
	}
}