#pragma once
#include "../IGUI_Container.h"

namespace GUI {
	class StackContainer : public IGUI_Container {
	public:
		StackContainer() = default;
		void render(Utils::Float2 origin, Utils::Float2 parentDimentions) const override;
	};
}
