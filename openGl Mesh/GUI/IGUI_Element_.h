#pragma once
#include "Utils/Float2.h"
#include "Utils/Float3.h"
#include "Utils/Float4.h"
#include "Utils/DrawData.h"

namespace GUI {
	// basic element effectivly just a rectangle
	// position is the bottom left
	class IGUI_Element {
	protected:
		IGUI_Element() : _position(), _dimentions(), _backgroundColour(), _cornerRadius() { }
		Utils::Float2 _position, _dimentions;
		Utils::Float4 _backgroundColour;
		float _cornerRadius;

		static void addQuad(Utils::DrawData& data, const Utils::Float2& offset, const Utils::Float2& dims, const Utils::Float4& colour) {
			data.push_back(Utils::Vertex({ offset.x, offset.y }, colour));
			data.push_back(Utils::Vertex({ offset.x + dims.x, offset.y }, colour));
			data.push_back(Utils::Vertex({ offset.x, offset.y + dims.y }, colour));
			data.push_back(Utils::Vertex({ offset.x + dims.x, offset.y + dims.y }, colour));
		}
	public:
		virtual Utils::DrawData getDrawData() const {
			Utils::DrawData d;
			// center quad
			Utils::Float2 offset = _position + _cornerRadius;
			Utils::Float2 dims = _dimentions - 2 * _cornerRadius;
			addQuad(d, offset, dims, _backgroundColour);

			if (_cornerRadius > 0) {
				// top quad
				offset = Utils::Float2(_cornerRadius, _dimentions.y - _cornerRadius);
				dims = Utils::Float2(_dimentions.x - 2 * _cornerRadius, _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);

				// bottom quad
				offset = Utils::Float2(_cornerRadius, 0);
				dims = Utils::Float2(_dimentions.x - 2 * _cornerRadius, _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);

				// left quad
				offset = Utils::Float2(0, _cornerRadius);
				dims = Utils::Float2(0, _dimentions.y - 2 * _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);

				// right quad
				offset = Utils::Float2(_dimentions.x - _cornerRadius, _cornerRadius);
				dims = Utils::Float2(0, _dimentions.y - 2 * _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);

				// TODO: add the rounded corners
			}
			return d;
		}

		inline void setDimentions(Utils::Float2 dims) { _dimentions = dims; }
		inline void setCornerRadius(float radius) { _cornerRadius = radius; }

		inline void setBackgroundColour(Utils::Float4 col) { _backgroundColour = col; }
		inline void setBackgroundColour(Utils::Float3 col) { _backgroundColour = Utils::Float4(col.r, col.g, col.b, 1); }
	};
}