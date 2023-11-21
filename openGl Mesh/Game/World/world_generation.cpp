#include "world_generation.h"
#include <gtc/noise.hpp>
#include "../../Helpers/Functions.h"
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
		noise = (noise + 1.0f) * .5f;
		value += noise * amplitude;
		accumulatedAmps += amplitude;
	}
	return value / accumulatedAmps;
}

void world_generation::createHeightMap(glm::vec2 chunkPos, unsigned int seed, HeightMap& res, unsigned int biome) {
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

	for (unsigned int x = 0; x < CHUNK_SIZE; x++)
	{
		for (unsigned int y = 0; y < CHUNK_SIZE; y++)
		{
			BlocksEncoded& encoded = res[columnIndex(x, y)];
			glm::vec2 worldPos = { x, y };
			worldPos += chunkPos;
			
			encoded = getColumn(worldPos, seed);
		}
	}
}

void world_generation::createHeightMapLimitedSamples(glm::vec2 chunkPos, unsigned int seed, HeightMap& res)
{
	// gets the height and adds the column if needed
	auto getHeight = [&](unsigned int x, unsigned int z) -> float {
		glm::vec2 worldPos(x, z);
		worldPos += chunkPos;
		BlocksEncoded encoded = getColumn(worldPos, seed);
		if (x >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		}
		else {
			res[columnIndex(x, z)] = encoded;
		}
		return encoded.getUnalteredHeight();
	};
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

	unsigned int sampleMult = CHUNK_SIZE / CHUNK_SAMPLES;

	// fills in the spaces with bilinear or sample
	for (unsigned int a = 0; a < sampleMult; a++)
	{
		for (unsigned int b = 0; b < sampleMult; b++)
		{
			unsigned int x = a * CHUNK_SAMPLES;
			unsigned int z = b * CHUNK_SAMPLES;

			float c00 = getHeight(x, z);
			float c10 = getHeight(x + CHUNK_SAMPLES, z);
			float c01 = getHeight(x, z + CHUNK_SAMPLES);
			float c11 = getHeight(x + CHUNK_SAMPLES, z + CHUNK_SAMPLES);

			for (unsigned int i = 0; i < CHUNK_SAMPLES; i++) {
				for (unsigned int j = 0; j < CHUNK_SAMPLES; j++) {
					float height = bilinearInterpolation((float)i / (float)CHUNK_SAMPLES, (float)j / (float)CHUNK_SAMPLES, c00, c01, c10, c11);
					// res[columnIndex(x + i, z + j)] = createColumn(height - 1);
					auto t0 = createColumn(height);
					if (t0.height() < 21) {
						int f = 0;
					}
					auto t1 = getColumn(glm::vec2(x + i + chunkPos.x, z + j + chunkPos.y), seed);
					res[columnIndex(x + i, z + j)] = t0;
				}
			}
		}
	}
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

unsigned int world_generation::heightOfColumn(glm::vec2 worldPos, const unsigned int seed)
{
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
	return result;
}

BlocksEncoded world_generation::getColumn(const glm::vec2& worldPos, unsigned int seed)
{
	float height = heightOfColumn(worldPos, seed);
	return createColumn(height);
}

BlocksEncoded world_generation::createColumn(unsigned int height)
{
	BlocksEncoded encoded;
	encoded.setUnalteredHeight(height);

	if (height < WATER_LEVEL) {
		// is a body of water
		unsigned int waterDepth = WATER_LEVEL - height;
		encoded.push(Block::STONE, height - 3);
		encoded.push(Block::SAND, 3);
		encoded.push(Block::WATER, waterDepth);
	}
	else {
		// is a land mass
		encoded.push(Block::STONE, height - 3);
		if (height > SNOW_LEVEL) {
			// is mountain
			encoded.push(Block::SNOW, 3);
		}
		else {
			encoded.push(Block::DIRT, 2);
			encoded.push(Block::GRASS, 1);
		}
	}

	/*if (height - 3 > 0) {
		encoded.push(Block::STONE, height -= 3);
	}
	if (height <= WATER_LEVEL) {
		encoded.push(Block::SAND, 3);
		encoded.push(Block::WATER, 20 - height - 2);
	}
	else if (height < WATER_LEVEL + 3) {
		encoded.push(Block::SAND, 3);
	}
	else if (height - 2 > 0) {
		encoded.push(Block::DIRT, 2);
		encoded.push(Block::GRASS, 1);
	}*/
	return encoded;
}
