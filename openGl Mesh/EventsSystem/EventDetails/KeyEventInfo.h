#pragma once
#include <list>

struct KeyEventInfo {
	// GLFW key code
	unsigned int _key;
	// GLFW release, press, repeat(hold)
	unsigned int _action;
	// GLFW bits corrispond to other button being held (shift, alt, caps, etc.)
	unsigned int _mod;
	KeyEventInfo() = default;
};