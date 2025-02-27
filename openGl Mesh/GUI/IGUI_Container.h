#pragma once
#include "IGUI_Element.h"
#include <list>

namespace GUI {
	class IGUI_Container : public IGUI_Element {
	public:
		inline unsigned int size() const { return _children.size(); }
		inline virtual void push(IGUI_Element* element) { _children.push_back(element); }

		virtual void render(Utils::Float2 origin, Utils::Float2 parentDimentions) const override {
			IGUI_Element::render(origin, parentDimentions);

			Utils::Float2 drawPosition = getDrawPosition(origin, parentDimentions);
			Utils::Float2 drawDimentions = getDrawDimentions(parentDimentions);

			for (IGUI_Element* child : _children) {
				child->render(drawPosition, drawDimentions);
			}
		}
	protected:
		IGUI_Container() = default;
		std::list<IGUI_Element*> _children;
	};
}