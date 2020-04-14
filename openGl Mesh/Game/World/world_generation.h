#pragma once
#include "constants.h"
#include "gtc/noise.hpp"
struct NoiseOptions {
	GLint octaves;
	GLfloat amplitude;
	GLfloat smoothness;
	GLfloat roughness;
	GLfloat offset;
};
class world_generation
{
public:
	static GLfloat heightAtPositon(const glm::vec2 pos, NoiseOptions options, GLuint seed);
	// static ChunkHeightMap genHeightMap(glm::vec3 chunkPos);
	static HeightMap createHeightMap(glm::vec2 chunkPos, GLuint seed, GLuint biome = 0);
	static std::vector<glm::vec2> getTreePositions(glm::vec2 chunkPos);
private:
	static glm::ivec2 treeCooldown;
};

