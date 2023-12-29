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
				offset = _position + Utils::Float2(_cornerRadius, _dimentions.y - _cornerRadius);
				dims = Utils::Float2(_dimentions.x - 2 * _cornerRadius, _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);

				// bottom quad
				offset = _position + Utils::Float2(_cornerRadius, 0);
				dims = Utils::Float2(_dimentions.x - 2 * _cornerRadius, _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);
				
				// left quad
				offset = _position + Utils::Float2(0, _cornerRadius);
				dims = Utils::Float2(_cornerRadius, _dimentions.y - 2 * _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);
				
				// right quad
				offset = _position + Utils::Float2(_dimentions.x - _cornerRadius, _cornerRadius);
				dims = Utils::Float2(_cornerRadius, _dimentions.y - 2 * _cornerRadius);
				addQuad(d, offset, dims, _backgroundColour);

				// top right
				float num_points = 10;
				float num_points_inc = (0.5f * 3.14f) / num_points;

				const float r_sq = _cornerRadius * _cornerRadius;
				const float two_r = 2.f * _cornerRadius;

				const Utils::Float2 base(1, 0);
				// float y = sqrtf(2.f * r_sq - 2.f * r_sq * cosf(theta));
				Utils::Float2 p1 = _position + _dimentions - _cornerRadius;
				Utils::Float2 p2 = p1 + base * _cornerRadius;
				for (unsigned int i = 1; i <= num_points; i++) {
					Utils::Float2 line = base.rotate(i * num_points_inc);
					line.normalise();
					line *= _cornerRadius;

					d.push_back(Utils::Vertex(p1, _backgroundColour));
					d.push_back(Utils::Vertex(p2, _backgroundColour));
					d.push_back(Utils::Vertex(p1 + line, _backgroundColour));

					p2 = p1 + line;
				}
			}
			return d;
		}

		inline void setPosition(Utils::Float2 pos) { _position = pos; }
		inline void setDimentions(Utils::Float2 dims) { _dimentions = dims; }
		inline void setCornerRadius(float radius) { _cornerRadius = radius; }

		inline void setBackgroundColour(Utils::Float4 col) { _backgroundColour = col; }
		inline void setBackgroundColour(Utils::Float3 col) { _backgroundColour = Utils::Float4(col.r, col.g, col.b, 1); }
	};
}