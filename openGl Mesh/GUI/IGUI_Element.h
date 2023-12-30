#pragma once
#include "Utils/Units.h"
#include "Utils/Float2.h"
#include "Utils/Float3.h"
#include "Utils/Float4.h"
#include "Utils/DrawData.h"
#include "Utils/Rendering/Buffer.h"

#define GUI_PIXEL_MODE

#ifdef GUI_PIXEL_MODE
#define GUI_DEFAULT_MODE PIXELS
#else
#define GUI_DEFAULT_MODE FRACTIONAL
#endif

namespace GUI {
	// basic element effectivly just a rectangle
	// position is the bottom left
	class IGUI_Element {
	private:
		Utils::Buffer _drawBuffer;
	protected:
		IGUI_Element();
		// all stored as either absolute pixel values or relative to parent
		Utils::Units<Utils::Float2> _position, _dimentions;
		// either absolute pixel or relative to this->_dimentions
		Utils::Units<float> _cornerRadius;

		Utils::Float4 _backgroundColour, _boarderColour;

		// if true causes the vertex data to be regenerated
		mutable bool _changed;
	public:

		// genreates vertex data for a unit rounded box (1 pixel by 1 pixel)
		virtual Utils::DrawData getDrawData() const;
		// origin is the position which this element is relative too (pixel space)
		virtual void render(Utils::Float2 origin, Utils::Float2 parentDimentions) const;

		void setPosition(Utils::Float2 pos, UNIT_MODE mode = GUI_DEFAULT_MODE);
		void setDimentions(Utils::Float2 dims, UNIT_MODE mode = GUI_DEFAULT_MODE);
		void setCornerRadius(float radius, UNIT_MODE mode = GUI_DEFAULT_MODE);

		inline void setBackgroundColour(Utils::Float4 col) { _backgroundColour = col; }
		inline void setBackgroundColour(Utils::Float3 col) { _backgroundColour = Utils::Float4(col.r, col.g, col.b, 1); }

		inline void setBoarderColour(Utils::Float4 col) { _boarderColour = col; }
		inline void setBoarderColour(Utils::Float3 col) { _boarderColour = Utils::Float4(col.r, col.g, col.b, 1); }
	};
}