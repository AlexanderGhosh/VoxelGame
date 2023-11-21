#pragma once
#include "../../Helpers/Constants.h"
#include <FastNoise/FastNoise.h>

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
	static void setUp();

	static float heightAtPositon(const glm::vec2 pos, NoiseOptions options, unsigned int seed);
	// static ChunkHeightMap genHeightMap(glm::vec3 chunkPos);
	static void createHeightMap(glm::vec2 chunkPos, unsigned int seed, HeightMap& res, unsigned int biome = 0);
	static void createHeightMapLimitedSamples(glm::vec2 chunkPos, unsigned int seed, HeightMap& res);
	static std::vector<glm::vec2> getTreePositions(glm::vec2 chunkPos);
	static unsigned int heightOfColumn(glm::vec2 worldPos, const unsigned int seed);
	static BlocksEncoded getColumn(const glm::vec2& worldPos, unsigned int seed);
private:
	static FastNoise::SmartNode<FastNoise::Simplex> noiseSource;
	static FastNoise::SmartNode<FastNoise::FractalFBm> noiseGenerator;
	static BlocksEncoded createColumn(unsigned int height);
	static glm::ivec2 treeCooldown;
	static std::vector<float> xs, ys;
};

