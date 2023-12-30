#pragma once
#include "../IGUI_Container.h"

namespace GUI {
	class StackContainer : public IGUI_Container {
	private:
		Utils::Units<float> _spacing;
	public:
		StackContainer();
		void render(Utils::Float2 origin, Utils::Float2 parentDimentions) const override;
		// spacing is used to seperate the children
		inline void setSpacing(float spacing, UNIT_MODE mode = GUI_DEFAULT_MODE) { _spacing.set(spacing, mode); }
	};
}
