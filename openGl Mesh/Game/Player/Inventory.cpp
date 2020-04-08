#include "Inventory.h"

Inventory::Inventory(glm::ivec2 size) : dimentions(size)
{
	contents.resize(dimentions.x);
	for (auto& column : contents) {
		column.resize(size.y);
	}
}

Item Inventory::getItemAt(glm::ivec2 pos)
{
	if (glm::any(glm::greaterThanEqual(pos, dimentions) || glm::lessThan(pos, { 0, 0 }))) return 0;
	return contents[pos.x][pos.y];
}
