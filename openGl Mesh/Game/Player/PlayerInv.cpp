#include "PlayerInv.h"

Item PlayerInv::getItemHotbar(GLubyte slot)
{
	return getItemAt({ slot, 0 });
}

Blocks PlayerInv::getBlockHotbar(GLubyte slot)
{
	return itemToBlock(getItemHotbar(slot));
}

GLubyte& PlayerInv::getHotbarSelected()
{
	return hotbarSlotSelected;
}

std::vector<Texture*> PlayerInv::getHotBarTextures()
{
	std::vector<Texture*> res;
	for (GLubyte x = 0; x < 9; x++) {
		Blocks block = getBlockHotbar(x);
		if (block == Blocks::AIR || block == Blocks::ERROR) {
			res.push_back(nullptr);
		}
		else {
			res.push_back(&BLOCKS[block].ItemTex);
		}
	}
	return res;
}

PlayerInv::PlayerInv() : Inventory({ 9, 4 })
{
	hotbarSlotSelected = 0;
}

void PlayerInv::setSlot(Item item, GLubyte slot)
{
	if (glm::any(glm::greaterThanEqual(glm::ivec2(slot, 0), dimentions) || glm::lessThan(glm::ivec2(slot, 0), { 0, 0 }))) return;
	contents[slot][0] = item;
}

void PlayerInv::setHotbarSelected(GLubyte slot)
{
	hotbarSlotSelected = slot;
}
