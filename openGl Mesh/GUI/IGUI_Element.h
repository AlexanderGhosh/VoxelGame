#pragma once
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
	// used to specify if the positoin and dimentios are given a sa % or pixels
	enum SIZE_MODE {
		PIXELS, FRACTIONAL
	};
	// basic element effectivly just a rectangle
	// position is the bottom left
	class IGUI_Element {
	private:
		Utils::Buffer _drawBuffer;
	protected:
		IGUI_Element();
		// all stored as a fraction of the screen dimentions
		Utils::Float2 _position, _dimentions, _cornerRadius;
		Utils::Float4 _backgroundColour, _boarderColour;
		mutable bool _changed;
	public:

		virtual Utils::DrawData getDrawData() const;
		virtual void render() const;

		void setPosition(Utils::Float2 pos, SIZE_MODE mode = GUI_DEFAULT_MODE);
		void setDimentions(Utils::Float2 dims, SIZE_MODE mode = GUI_DEFAULT_MODE);
		void setCornerRadius(float radius, SIZE_MODE mode = GUI_DEFAULT_MODE);

		inline void setBackgroundColour(Utils::Float4 col) { _backgroundColour = col; }
		inline void setBackgroundColour(Utils::Float3 col) { _backgroundColour = Utils::Float4(col.r, col.g, col.b, 1); }

		inline void setBoarderColour(Utils::Float4 col) { _boarderColour = col; }
		inline void setBoarderColour(Utils::Float3 col) { _boarderColour = Utils::Float4(col.r, col.g, col.b, 1); }
	};
}