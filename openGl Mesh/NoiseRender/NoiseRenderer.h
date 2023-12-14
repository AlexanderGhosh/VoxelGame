#pragma once
#include <list>
#include "NoiseRenderData.h"
class NoiseRenderer
{
private:
	std::list<NoiseRenderData> data_;
public:
	void add(const NoiseRenderData& data);
	void render();
};

