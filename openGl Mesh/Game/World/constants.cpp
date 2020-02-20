#include "constants.h"
const std::array<Buffer*, 6> FACES = {
	FaceMesh({ glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5), glm::vec3(0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5) },   // FRONT
	{ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) },
	{ glm::vec3(-1, -1, 1), glm::vec3(-1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5) },  // BACK
	{ glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1) },
	{ glm::vec3(-1, 1, -1), glm::vec3(-1), glm::vec3(1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5), glm::vec3(-0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5, 0.5, 0.5) },  // LEFT
	{ glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0) },
	{ glm::vec3(1, -1, -1), glm::vec3(1, -1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1) }).getBuffer(),

	FaceMesh({ glm::vec3(0.5), glm::vec3(0.5, 0.5, -0.5),glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5) },     // RIGHT
	{ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) },
	{ glm::vec3(-1, -1, 1), glm::vec3(-1, 1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5),glm::vec3(0.5), glm::vec3(0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, -0.5) },	  // TOP
	{ glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) },
	{ glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1), glm::vec3(-1, -1, -1), glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5), glm::vec3(0.5, -0.5, -0.5),glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5) },   // BOTTOM
	{ glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0) },
	{ glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1), glm::vec3(1), glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1) }).getBuffer()
}; 
const std::vector<Texture*>TEXTURES = {
	new Texture("grass/hd", ""),
	new Texture("player/bottom", ""),
	new Texture("player/top", ""),
	new Texture("skybox", ""),
	new Texture("stone", ""),
	new Texture("dirt", ""),
	new Texture("water", "")
};
const std::vector<Shader*>SHADERS = {
	new Shader("block2"),
	new Shader("block3"),
	new Shader("skybox")
};
GLint getBlockIndex(glm::vec3 position) {
	position.y = std::abs(position.y);
	int index = position.x + position.z * CHUNK_SIZE + position.y * CHUNK_AREA;
	if (index >= CHUNK_VOLUME) index *= -1;
	return index;
}
GLint getChunkIndex(glm::vec3 position, GLboolean absalute, GLboolean reduced) {
	if (!reduced) {
		position /= CHUNK_SIZE;
	}
	if (!absalute) {
		position.y += 1;
		position.x += RENDER_DISTANCE / 2;
		position.z += RENDER_DISTANCE / 2;
		position = glm::abs(position);
	}
	int index = position.x + position.z * RENDER_DISTANCE + position.y * RENDERED_AREA;
	if (index >= RENDERED_VOLUME) index *= -1;
	return index;
}