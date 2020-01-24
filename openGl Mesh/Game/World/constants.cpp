#include "constants.h"


GLuint getBlockIndex(glm::vec3 position) {
	/*
	GLuint index;
	std::stringstream(std::to_string(position.x) + std::to_string(position.y) + std::to_string(position.z)) >> index;
	return index;*/
	int index = position.x + position.z * CHUNK_SIZE + position.y * position.y;
	if (index < 0) return 0;
	return index;
}