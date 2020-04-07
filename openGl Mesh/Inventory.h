#pragma once
#include "Game/World/constants.h"
class Inventory
{
private: // properties
	glm::ivec2 dimentions;
	std::vector<std::vector<Item>> contence;
public:
	// Inventory();
	// Inventory();
	Item getItemAt(glm::ivec2 pos);
};

