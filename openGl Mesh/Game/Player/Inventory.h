#pragma once
#include "../World/constants.h"
class Inventory
{
protected: // properties
	glm::ivec2 dimentions;
	std::vector<std::vector<Item>> contents;
public:
	Inventory();
	Inventory(glm::ivec2 size);
	Item getItemAt(glm::ivec2 pos);
};

