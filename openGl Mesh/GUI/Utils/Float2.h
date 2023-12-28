#pragma once

namespace GUI {
	namespace Utils {
		class Float2 {
		public:
			float x, y;
			Float2() : x(0), y(0) { }
			Float2(float x, float y) : x(x), y(y) { }

			friend Float2 operator+(const Float2& a, const Float2& b) {
				return Float2(a.x + b.x, a.y + b.y);
			}
			friend Float2 operator+(const Float2& a, const float& b) {
				return Float2(a.x + b, a.y + b);
			}

			friend Float2 operator-(const Float2& a, const Float2& b) {
				return Float2(a.x - b.x, a.y - b.y);
			}
			friend Float2 operator-(const Float2& a, const float& b) {
				return Float2(a.x - b, a.y - b);
			}
		};
		/*Float2 operator+(const Float2& a, const Float2& b) {
			return Float2(a.x + b.x, a.y + b.y);
		}
		Float2 operator+(const Float2& a, const float& b) {
			return Float2(a.x + b, a.y + b);
		}

		Float2 operator-(const Float2& a, const Float2& b) {
			return Float2(a.x - b.x, a.y - b.y);
		}
		Float2 operator-(const Float2& a, const float& b) {
			return Float2(a.x - b, a.y - b);
		}*/
	}
}