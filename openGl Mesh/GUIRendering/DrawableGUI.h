#pragma once
#include <forward_list>
#include <ext/vector_float2.hpp>
#include "GUIBuffer.h"

class DrawableGUI
{
public:
	DrawableGUI();
	void render(const glm::vec2 windowDims);
	// manualy moves buffer
	void add(GUIBuffer& buffer);
	void clear();
private:
	std::forward_list<GUIBuffer> _buffers;
};

