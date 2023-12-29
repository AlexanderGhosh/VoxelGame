#pragma once
#include "Utils/DrawData.h"
#include "Utils/Float2.h"

namespace GUI {
	class IGUI_Container;
	class GUI_Window {
	public:
		GUI_Window() : _root() { }
		Utils::DrawData getDrawData() const;

		void setRoot(IGUI_Container* container);
		static Utils::Float2 windowDimentions;
	private:
		IGUI_Container* _root;
	};
}