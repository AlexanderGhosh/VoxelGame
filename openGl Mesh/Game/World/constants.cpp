#include "constants.h"

const std::vector<Texture*>TEXTURES = {
	new Texture("grass", ""),
	new Texture("player/bottom", ""),
	new Texture("player/top", "")
};
GLint getBlockIndex(glm::vec3 position) {
	position.y = std::abs(position.y);
	int index = position.x + position.z * CHUNK_SIZE + position.y * CHUNK_AREA;
	if (index >= CHUNK_VOLUME) index *= -1;
	return index;
}