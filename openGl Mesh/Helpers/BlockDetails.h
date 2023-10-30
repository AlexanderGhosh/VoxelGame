#pragma once
#include <string>
#include "../Textures/Texture.h"

struct BlockDetails {
	std::string Name;
	Texture* Tex;
	Texture ItemTex;
	bool isTransparant;

	BlockDetails() = default;
};
