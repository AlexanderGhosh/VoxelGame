#pragma once
#include "IGUI_Element_.h"
#include <list>

namespace GUI {
	class IGUI_Container : public IGUI_Element {
	public:
		inline unsigned int size() const { return _children.size(); }
		inline void push(IGUI_Element* element) { _children.push_back(element); }
		template<typename T>
		inline void push(T& element) { _children.push_back(&element); }
	protected:
		IGUI_Container() = default;
		std::list<IGUI_Element*> _children;
	};
}