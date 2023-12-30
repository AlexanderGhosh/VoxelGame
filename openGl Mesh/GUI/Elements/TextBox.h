#pragma once
#include "../IGUI_Element.h"
#include <string>

namespace GUI {
	class TextBox : public IGUI_Element {
	public:
		enum LAYOUT_TYPE {
			NONE, CENTERED, VERTICAL, HORIZONTAL
		};
		TextBox();
		Utils::DrawData getDrawData() const override;

		void setText(const std::string& text);
		// used to position the text relative to the draw position and draw dimentions
		void setPadding(Utils::Float2 padding, UNIT_MODE mode = GUI_DEFAULT_MODE);

		void render(Utils::Float2 origin, Utils::Float2 parentDimentions) const override;

		// used to position text relative to the rect 
		// is calculated at render time
		inline void setLayoutType(LAYOUT_TYPE type) { _layoutType = type; }
		// when true ensures that the dimentions is at least large enough to fully contain the text
		inline void ensureMinmumSize(bool enforce) { _ensureMinmumSize = enforce; }

		inline void setTextColour(Utils::Float3 col) { setTextColour({ col.r, col.g, col.b, 1 }); }
		inline void setTextColour(Utils::Float4 col) { _textColour = col; }
	private:
		std::string _text;
		Utils::Units<Utils::Float2> _textPadding;
		Utils::Float4 _textColour;
		LAYOUT_TYPE _layoutType;
		bool _ensureMinmumSize;
	};
}