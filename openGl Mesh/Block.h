#pragma once

class Block {
public:
	Block() = default;
	bool isTransparent;
	bool isDynamic;
	unsigned int materialIndex;

	bool operator==(const Block& other) const noexcept {
		return isTransparent == other.isTransparent && isDynamic == other.isDynamic && materialIndex == other.materialIndex;
	}
	bool operator!=(const Block& other) const noexcept {
		return !(*this == other);
	}
};



#define B_ERROR (BLOCKS[0])
#define B_AIR (BLOCKS[1])
#define B_GRASS (BLOCKS[2])
#define B_DIRT (BLOCKS[3])
#define B_STONE (BLOCKS[4])
#define B_SAND (BLOCKS[5])
#define B_WATER (BLOCKS[6])
#define B_SNOW (BLOCKS[7])
#define B_GRAVEL (BLOCKS[8])
#define B_GLASS (BLOCKS[9])