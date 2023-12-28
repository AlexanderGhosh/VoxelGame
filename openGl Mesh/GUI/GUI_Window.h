#pragma once
#include "Utils/DrawData.h"

namespace GUI {
	class IGUI_Container;
	class GUI_Window {
	public:
		GUI_Window() = default;
		Utils::DrawData getDrawData() const;

		void setRoot(IGUI_Container* container);
	private:
		IGUI_Container* _root;
	};
}