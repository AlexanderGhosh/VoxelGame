#pragma once
#include "Utils/DrawData.h"
#include "Utils/Float2.h"

namespace GUI {
	class IGUI_Container;
	class GUI_Window {
	public:
		GUI_Window() : _root() { }

		void render() const;

		void setRoot(IGUI_Container* container);
		static Utils::Float2 windowDimentions;
		static unsigned int elementShader;
	private:
		IGUI_Container* _root;
	};
}