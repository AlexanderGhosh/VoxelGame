#pragma once
#include "../../Helpers/Constants.h"
#include <FastNoise/FastNoise.h>

struct NoiseOptions {
	unsigned int octaves;
	float frequency;
};
class world_generation
{
public:
	static void setUp();

	// returns the raw heights
	static std::vector<float> getRawHeightsPadded(glm::vec2 chunkPos, unsigned int _seed);

	// static ChunkHeightMap genHeightMap(glm::vec3 chunkPos);
	static void createHeightMap(glm::vec2 chunkPos, unsigned int _seed, HeightMap& res, unsigned int biome = 0);
	static void createHeightMapLimitedSamples(glm::vec2 chunkPos, unsigned int _seed, HeightMap& res);
	static unsigned int heightOfColumn(glm::vec2 worldPos, const unsigned int _seed);
	static BlocksEncoded getColumn(const glm::vec2& worldPos, unsigned int _seed);
	static BlocksEncoded createColumn(unsigned int height);
private:
	static std::vector<float> getRawHeights(glm::vec2 chunkPos, unsigned int _seed);
	static unsigned int noiseToHeight(const float noise, const FastNoise::OutputMinMax& minMax);

	static FastNoise::SmartNode<FastNoise::Simplex> noiseSource;
	static FastNoise::SmartNode<> noiseGenerator;

	static glm::ivec2 treeCooldown;
	static std::vector<float> xs, ys;
	static NoiseOptions options;
};

