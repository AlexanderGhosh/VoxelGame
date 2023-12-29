#pragma once

namespace GUI::Utils {
	// used to specify if the positoin and dimentios are given a sa % or pixels
	enum UNIT_MODE {
		PIXELS, FRACTIONAL
	};
	template<typename T>
	class Units {
	private:
		T _value;
		UNIT_MODE _units;
		
	public:
		Units() : _value(), _units() { }
		inline void setPixel(T value) {
			_units = PIXELS;
			_value = value;
		}
		inline void setFractional(T value) {
			_units = FRACTIONAL;
			_value = value;
		}

		T getPixelValue(Units<T> max) const {
			switch (_units)
			{
			case GUI::Utils::PIXELS:
				return _value;
			case GUI::Utils::FRACTIONAL:
				return _value * max;
			}
		}

		T getFractionalValue(Units<T> max) const {
			switch (_units)
			{
			case GUI::Utils::PIXELS:
				return _value / max;
			case GUI::Utils::FRACTIONAL:
				return _value;
			}
		}
	};
}