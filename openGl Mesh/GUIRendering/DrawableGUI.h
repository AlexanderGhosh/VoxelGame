#pragma once
#include <forward_list>
#include "GUIBuffer.h"

class DrawableGUI
{
public:
	DrawableGUI();
	void render();
	// manualy moves buffer
	void add(GUIBuffer& buffer);
	void clear();
private:
	std::forward_list<GUIBuffer> _buffers;
};

