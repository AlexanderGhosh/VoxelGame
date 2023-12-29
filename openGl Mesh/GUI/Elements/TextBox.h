#pragma once
#include "../IGUI_Element.h"
#include <string>

namespace GUI {
	class TextBox : public IGUI_Element {
	public:
		TextBox() = default;
		Utils::DrawData getDrawData() const override;
	private:
		std::string _text;
	};
}