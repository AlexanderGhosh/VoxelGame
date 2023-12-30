#pragma once

namespace GUI {
	enum UNIT_MODE {
		PIXELS, FRACTIONAL
	};
	namespace Utils {
		template<typename T>
		class Units {
		private:
			T _value;
			UNIT_MODE _units;

		public:
			Units() : _value(), _units() { }
			template<typename ...Types>
			Units(UNIT_MODE units, Types... args) : _value(args...), _units(units) { }

			inline void set(T value, UNIT_MODE units) {
				_value = value;
				_units = units;
			}

			inline void setPixel(T value) {
				_units = PIXELS;
				_value = value;
			}
			inline void setFractional(T value) {
				_units = FRACTIONAL;
				_value = value;
			}

			T getPixelValue(T max) const {
				switch (_units)
				{
				case GUI::PIXELS:
					return _value;
				case GUI::FRACTIONAL:
					return _value * max;
				}
			}

			T getFractionalValue(T max) const {
				switch (_units)
				{
				case GUI::PIXELS:
					return _value / max;
				case GUI::FRACTIONAL:
					return _value;
				}
			}

			inline bool isPixelValue() const { return _units == PIXELS; }
			inline bool isFractionalValue() const { return _units == FRACTIONAL; }
		};
	}
	// used to specify if the positoin and dimentios are given a sa % or pixels
	
}