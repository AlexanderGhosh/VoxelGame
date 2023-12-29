#pragma once
#include "IGUI_Element.h"
#include <list>

namespace GUI {
	class IGUI_Container : public IGUI_Element {
	public:
		inline unsigned int size() const { return _children.size(); }
		inline void push(IGUI_Element* element) { _children.push_back(element); }
		template<typename T>
		inline void push(T& element) { _children.push_back(&element); }

		virtual void render() const override {
			// IGUI_Element::render();
			for (IGUI_Element* child : _children) {
				child->render();
			}
		}
	protected:
		IGUI_Container() = default;
		std::list<IGUI_Element*> _children;
	};
}