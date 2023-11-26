#include "world_generation.h"
#include "../../Helpers/Functions.h"
#include "../../Helpers/BlocksEncoded.h"
#include <iostream>


FastNoise::SmartNode<FastNoise::Simplex> world_generation::noiseSource;
FastNoise::SmartNode<FastNoise::FractalFBm> world_generation::noiseGenerator;
std::vector<float> world_generation::xs(CHUNK_AREA), world_generation::ys(CHUNK_AREA);
glm::ivec2 world_generation::treeCooldown = glm::vec2(4);
NoiseOptions world_generation::options = {};

void world_generation::setUp()
{
	options.octaves = 5;
	options.frequency = 0.7;
	
	noiseSource = FastNoise::New<FastNoise::Simplex>();
	noiseGenerator = FastNoise::New<FastNoise::FractalFBm>();

	noiseGenerator->SetSource(noiseSource);
	noiseGenerator->SetOctaveCount(options.octaves);
	noiseGenerator->SetLacunarity(1.5);
	unsigned int x = 0, y = 0;
	for (float i = 0; i < CHUNK_SIZE; i++) {
		for (float j = 0; j < CHUNK_SIZE; j++) {
			xs[x++] = j * NOISE_FACTOR * options.frequency;
			ys[y++] = i * NOISE_FACTOR * options.frequency;
		}
	}

}

void world_generation::createHeightMap(glm::vec2 chunkPos, unsigned int seed, HeightMap& res, unsigned int biome) {
	std::vector<float> noiseOutput(CHUNK_AREA);

	chunkPos *= NOISE_FACTOR * options.frequency;
	auto minMax = noiseGenerator->GenPositionArray2D(noiseOutput.data(), CHUNK_AREA, xs.data(), ys.data(), chunkPos.x, chunkPos.y, seed);

	for (unsigned int i = 0; i < CHUNK_AREA; i++)
	{
		unsigned int height = noiseToHeight(noiseOutput[i], minMax);
		res[i] = createColumn(height);
	}
}

void world_generation::createHeightMapLimitedSamples(glm::vec2 chunkPos, unsigned int seed, HeightMap& res)
{
}

unsigned int world_generation::heightOfColumn(glm::vec2 worldPos, const unsigned int seed)
{
	worldPos *= NOISE_FACTOR * options.frequency;
	float noise = noiseGenerator->GenSingle2D(worldPos.x, worldPos.y, seed);
	FastNoise::OutputMinMax minMax;
	unsigned int height = noiseToHeight(noise, minMax);
	return height;
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
		int h = randInt(2, 4);
		encoded.push(Block::STONE, height - h);
		if (height > SNOW_LEVEL) {
			// is mountain
			encoded.push(Block::SNOW, h);
		}
		else {
			encoded.push(Block::DIRT, h - 1);
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

unsigned int world_generation::noiseToHeight(const float noise, const FastNoise::OutputMinMax& minMax)
{
	return (noise + 1.f) * .5f * 50;
}
