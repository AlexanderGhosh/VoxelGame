#pragma once
#include "../IGUI_Container.h"
namespace GUI {
	class BasicContainer : public IGUI_Container {
	public:
		BasicContainer() = default;
		Utils::DrawData getDrawData() const override;
	};
}