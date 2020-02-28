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
	new Texture("grass/hd/debug", ""),
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
//GLint getChunkIndex(glm::vec3 position, GLboolean absalute, GLboolean reduced) {
//	if (!reduced) {
//		position /= CHUNK_SIZE;
//	}
//	if (!absalute) {
//		position.y += 1;
//		position.x += INITALL_VIEW / 2;
//		position.z += INITALL_VIEW / 2;
//		position = glm::abs(position);
//	}
//	int index = position.x + position.z * INITALL_VIEW + position.y * INITALL_VIEW_AREA;
//	if (index >= INITALL_VIEW_VOLUME) index *= -1;
//	return index;
//}
void reduceToMultiple(glm::ivec3& victim, GLuint multiple, const char* overload) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0 || victim.y >= 0) {
		victim.y -= 1;
	}
	while (victim.z % multiple != 0) {
		victim.z -= 1;
	}
}
glm::ivec3 reduceToMultiple(glm::ivec3 victim, GLuint multiple) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0 || victim.y >= 0) {
		victim.y -= 1;
	}
	while (victim.z % multiple != 0) {
		victim.z -= 1;
	}
	return victim;
}
GLint reduceToMultiple(GLfloat victim, GLuint multiple) {
	victim = std::round(victim);
	while ((GLint)victim % multiple != 0) {
		victim--;
	}
	return victim;
}
//GLuint getChunkIndex(glm::vec3 pos, GLboolean rectify, GLboolean raw, const char* overload) {
//	if (rectify) {
//		pos /= CHUNK_SIZE;
//		pos.y += 1;
//		pos.x += INITALL_VIEW / 2;
//		pos.z += INITALL_VIEW / 2;
//		pos = glm::abs(pos);
//	}
//	GLuint index = pos.x + pos.z * INITALL_VIEW + pos.y * INITALL_VIEW_AREA; // needs a new formula
//	if (raw) return index;
//	if (index >= INITALL_VIEW_VOLUME) index *= -1;
//	return index;
//}