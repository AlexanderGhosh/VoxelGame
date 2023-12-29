#pragma once
#include "../Float2.h"

namespace GUI::Utils::Text {
	class Glyph {
	public:
		// the id of the texture (is deleted by the GlyphRendering
		unsigned int textureID;
		// the total width and height of the glyph (and quad to draw)
		Float2 size;
		// the offset from the position to draw from to the top right of the quad
		Float2 bearing;
		// how far to travel to the start of the next glyph draw (x is to the next char, y to next line)
		Float2 advance;

		// returns the offset to the bottom left fo the quad to draw
		Float2 getOffset() const {
			return Float2(-bearing.x, bearing.y - size.y);
		}

		Glyph() : size(), bearing(), advance() { }
	};
}