#pragma once
#include <string>
#include "../Textures/Texture.h"

enum class Block : unsigned char;
struct BlockDetails {
	Block type;
	bool isTransparant;
	bool isDynamic;

	BlockDetails() = default;
};
