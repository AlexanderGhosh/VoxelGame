#pragma once
#include <list>

struct KeyDownEventInfo {
	struct KeyInfo {
		// GLFW key code
		unsigned int _key;
		// true if the key was held in the alst frame
		bool _held;
	};
	KeyDownEventInfo() = default;
	// the GLFW key code of all the key currently down
	std::list<KeyInfo> _keys;
};