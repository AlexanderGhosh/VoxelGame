#pragma once
#include "constants.h"
#include "gtc/noise.hpp"
struct NoiseOptions {
	int octaves;
	float amplitude;
	float smoothness;
	float roughness;
	float offset;
};
class world_generation
{
public:
	static float heightAtPositon(const glm::vec2 pos, NoiseOptions options, unsigned int seed);
	// static ChunkHeightMap genHeightMap(glm::vec3 chunkPos);
	static HeightMap createHeightMap(glm::vec2 chunkPos, unsigned int seed, unsigned int biome = 0);
	static std::vector<glm::vec2> getTreePositions(glm::vec2 chunkPos);
private:
	static glm::ivec2 treeCooldown;
};

