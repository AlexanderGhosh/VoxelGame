#pragma once
#include "Inventory.h"
class PlayerInv : public Inventory
{
private: // properties
	GLubyte hotbarSlotSelected;

public: // functions
	PlayerInv();
	
	void setSlot(Item item, GLubyte slot);
	void setHotbarSelected(GLubyte slot);

	Item getItemHotbar(GLubyte slot);
	Blocks getBlockHotbar(GLubyte slot);
	GLubyte& getHotbarSelected();
	std::vector<Texture*> getHotBarTextures();
};

