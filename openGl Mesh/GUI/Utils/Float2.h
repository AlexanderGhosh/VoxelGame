#pragma once
#include <math.h>

namespace GUI {
	namespace Utils {
		class Float2 {
		public:
			float x, y;
			Float2() : x(0), y(0) { }
			Float2(float x, float y) : x(x), y(y) { }

			Float2 rotate(float theta) const {
				Float2 res;
				res.x = x * cosf(theta) - y * sinf(theta);
				res.y = x * sinf(theta) + y * cosf(theta);
				return res;
			}

			void normalise() {
				float m = magnitude();
				x /= m;
				y /= m;
			}

			float magnitude() const {
				return sqrtf(dot(*this));
			}

			float dot(const Float2& other) const {
				return x * other.x + y * other.y;
			}


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

			friend Float2 operator*(const Float2& a, const Float2& b) {
				return Float2(a.x * b.x, a.y * b.y);
			}

			friend Float2 operator*(const Float2& a, const float& b) {
				return Float2(a.x * b, a.y * b);
			}

			Float2& operator+=(const Float2& other) {
				x += other.x;
				y += other.y;
				return *this;
			}
			Float2& operator*=(const Float2& other) {
				x *= other.x;
				y *= other.y;
				return *this;
			}
			Float2& operator*=(float other) {
				x *= other;
				y *= other;
				return *this;
			}
			Float2& operator/=(const Float2& other) {
				x /= other.x;
				y /= other.y;
				return *this;
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