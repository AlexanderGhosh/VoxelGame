#pragma once
#include "../IGUI_Element.h"
#include <string>

namespace GUI {
	class TextBox : public IGUI_Element {
	public:
		enum LAYOUT_TYPE {
			NONE, CENTERED, VERTICAL, HORIZONTAL
		};
		TextBox() = default;
		Utils::DrawData getDrawData() const override;

		void setText(const std::string& text);
		// used to position the text relative to the draw position and draw dimentions
		void setPadding(Utils::Float2 padding, UNIT_MODE mode = GUI_DEFAULT_MODE);

		void render(Utils::Float2 origin, Utils::Float2 parentDimentions) const override;

		// used to position text relative to the rect 
		// is calculated at render time
		inline void setLayoutType(LAYOUT_TYPE type) { _layoutType = type; }
	private:
		std::string _text;
		Utils::Units<Utils::Float2> _textPadding;
		LAYOUT_TYPE _layoutType;
	};
}