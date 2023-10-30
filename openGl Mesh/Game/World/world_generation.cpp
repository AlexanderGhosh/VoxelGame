#include "world_generation.h"
#include <gtc/noise.hpp>
#include "../../Helpers/BlocksEncoded.h"

glm::ivec2 world_generation::treeCooldown = glm::vec2(4);

float world_generation::heightAtPositon(glm::vec2 pos, NoiseOptions options, unsigned int seed) {

	// Begin iterating through the octaves
	float value = 0;
	float accumulatedAmps = 0;
	for (int i = 0; i < options.octaves; i++) {
		float frequency = glm::pow(2.0f, i);
		float amplitude = glm::pow(options.roughness, i);

		glm::vec2 xy = (pos * frequency) / options.smoothness;
		xy += seed;

		float noise = glm::simplex(glm::vec3{ xy.x, xy.y, seed });
		noise = (noise + 1.0f) / 2.0f;
		value += noise * amplitude;
		accumulatedAmps += amplitude;
	}
	return value / accumulatedAmps;
}

HeightMap world_generation::createHeightMap(glm::vec2 chunkPos, unsigned int seed, unsigned int biome) {
	// doesn't generate caves
	NoiseOptions firstNoise{};
	firstNoise.amplitude = 105.0f;
	firstNoise.octaves = 6;
	firstNoise.smoothness = 205.0f;
	firstNoise.roughness = 0.58f;
	firstNoise.offset = 18.0f;

	NoiseOptions secondNoise{};
	secondNoise.amplitude = 20.0f;
	secondNoise.octaves = 4;
	secondNoise.smoothness = 200.0f;
	secondNoise.roughness = 0.45f;
	secondNoise.offset = 0.0f;

	HeightMap res;

	for (unsigned int x = 0; x < CHUNK_SIZE; x++)
	{
		for (unsigned int y = 0; y < CHUNK_SIZE; y++)
		{
			BlocksEncoded& encoded = res[columnIndex(x, y)];
			glm::vec2 worldPos = { x, y };
			worldPos += chunkPos;
			float height = heightAtPositon(worldPos, firstNoise, seed);
			float height2 = heightAtPositon(worldPos, secondNoise, seed);
			float result = height * height2;
			result *= firstNoise.amplitude + firstNoise.offset - 5;
			result = abs(result);
			if (worldPos == glm::vec2(0)) {
				worldPos.x++;
				height = heightAtPositon(worldPos, firstNoise, seed);
				height2 = heightAtPositon(worldPos, secondNoise, seed);
				result = height * height2;
				result *= firstNoise.amplitude + firstNoise.offset - 5;
				result = abs(result);
			}

			float result_orig = result;

			if (result < 2) {
				encoded.push(Block::WATER, result + 1);
				break;
			}
			if (result - 3 > 0) {
				encoded.push(Block::STONE, result -= 3);
			}
			if (result_orig <= 20) {
				encoded.push(Block::SAND, 3);
				encoded.push(Block::WATER, 20 - result - 2);
				continue;
			}
			if (result_orig < 23) {
				encoded.push(Block::SAND, 3);
				continue;
			}
			if (result - 2 > 0) {
				encoded.push(Block::DIRT, 2);
			}
			encoded.push(Block::GRASS, 1);
		}
	}
	return res;
}

std::vector<glm::vec2> world_generation::getTreePositions(glm::vec2 chunkPos)
{
	std::vector <glm::vec2> trees;
	treeCooldown = { 4, 4 };
	for (unsigned int x = 2; x < CHUNK_SIZE-2; x++)
	{
		for (unsigned int z = 2; z < CHUNK_SIZE-2; z++)
		{
			if (rand() / double(RAND_MAX) < 0.05 && glm::all(glm::lessThanEqual(treeCooldown, glm::ivec2(0)))) {
				// place tree at
				trees.push_back({ x, z });
				treeCooldown = { 4, 4 };
			}
			treeCooldown.y--;
		}
		treeCooldown.x--;
	}
	return trees;
}
