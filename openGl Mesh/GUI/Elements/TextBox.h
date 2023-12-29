#pragma once
#include "../IGUI_Element.h"
#include <string>

namespace GUI {
	class TextBox : public IGUI_Element {
	public:
		TextBox() = default;
		Utils::DrawData getDrawData() const override;

		void setText(const std::string& text);
		void setPadding(Utils::Float2 padding, UNIT_MODE mode = GUI_DEFAULT_MODE);

		void render(Utils::Float2 origin, Utils::Float2 parentDimentions) const override;
	private:
		std::string _text;
		// used to position the text relative to the _position
		Utils::Float2 _textPadding;
	};
}