#include "world_generation.h"
GLuint world_generation::seed = 0;
GLubyte world_generation::octaves = 1;
std::vector<glm::vec2> world_generation::frequencies = { glm::vec2(1.0f) };
GLfloat world_generation::redistribution = 1.0f;
world_generation::world_generation() {
	seed = 1;
	octaves = 1;
	redistribution = 1;
	frequencies = { glm::vec2(1.0f) };
}
world_generation::world_generation(GLuint seed, GLubyte octaves, GLfloat redist, std::vector<glm::vec2> freqs) {
	this->seed = seed;
	this->octaves = octaves;
	redistribution = redist;
	frequencies = freqs;
	if (frequencies.size() < this->octaves) {
		frequencies.push_back(glm::vec2(1.0f));
	}
}
GLubyte world_generation::heightAtPositon(glm::vec2 pos) {
	glm::vec2 inp = pos / (GLfloat)seed - 0.5f;
	GLfloat mult = 1.0f;
	GLfloat sum = 0.0f;
	for (GLubyte i = 0; i < octaves; i++) {
		GLfloat raw = noise(inp * frequencies[i]);
		// raw += 1.0f; // range 0 - 2
		// raw /= 2.0f; // range 0 - 1
		// raw *= CHUNK_SIZE; // range 0 - CHUNK_SIZE
		sum += mult * raw;
		mult /= 2.0f;
	}
	sum = std::pow(sum, redistribution);
	/*sum = std::lround(sum);
	if (sum >= CHUNK_SIZE) sum = CHUNK_SIZE - 1;
	if (sum < 0) sum++;*/
	sum *= 10;
	sum = std::abs(sum);
	if (sum > CHUNK_SIZE) sum = CHUNK_SIZE - 1;
	sum = std::lround(sum);
	return sum;
}
GLubyte world_generation::heightAtPositon(GLfloat x, GLfloat y) {
	return heightAtPositon(glm::vec2(x, y));
}
GLfloat world_generation::noise(glm::vec2 pos) {
	return glm::perlin(pos);
}
GLfloat world_generation::noise(GLfloat x, GLfloat y) {
	return glm::perlin(glm::vec2(x, y));
}
ChunkHeightMap world_generation::genHeightMap(glm::vec3 chunkPos) {
	ChunkHeightMap res;
	for (GLubyte x = 0; x < CHUNK_SIZE; x++) {
		for (GLubyte z = 0; z < CHUNK_SIZE; z++) {
			glm::vec2 pos(x, z);
			pos.x += chunkPos.x;
			pos.y += chunkPos.z;
			res[x][z] = heightAtPositon(pos);
		}
	}
	return res;
}
HeightMap world_generation::createHeightMap(glm::vec3 chunkPos, GLboolean hasCaves) {
	HeightMap res;
	if (hasCaves) {
		// make caves
	}
	else {
		for (GLubyte x = 0; x < CHUNK_SIZE; x++) {
			for (GLubyte z = 0; z < CHUNK_SIZE; z++) {
				glm::vec2 pos(x, z);
				pos.x += chunkPos.x;
				pos.y += chunkPos.z;
				res[x][z].push_back({ 1, heightAtPositon(pos) });
			}
		}
	}
	return res;
}