#include "world_generation.h"
#include <iostream>
#include "../../Block.h"
#include "../../Helpers/Constants.h"
#include "../../Helpers/Functions.h"
#include "../../Helpers/BlocksEncoded.h"


FastNoise::SmartNode<FastNoise::Simplex> world_generation::noiseSource;
FastNoise::SmartNode<> world_generation::noiseGenerator;
// uses padded for the sake of 'getRawHeightsPadded'
std::vector<float> world_generation::xs(CHUNK_AREA_PADDED), world_generation::ys(CHUNK_AREA_PADDED);
glm::ivec2 world_generation::treeCooldown(4);
NoiseOptions world_generation::options = {};

void world_generation::setUp()
{
	// AgAAAIA/
	options.octaves = 5;
	options.frequency = 1;
	
	noiseSource = FastNoise::New<FastNoise::Simplex>();
	noiseGenerator = FastNoise::New<FastNoise::FractalFBm>(); 
#ifdef FLAT_TERRAIN
	noiseGenerator = FastNoise::NewFromEncodedNodeTree("AAAAAAA/");
#else
	noiseGenerator = FastNoise::NewFromEncodedNodeTree("EQACAAAAAAAgQBAAAAAAQBkAEwDD9Sg/DQAEAAAAAAAgQAkAAGZmJj8AAAAAPwEEAAAAAAAAAIA/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAM3MTD4AMzMzPwAAAAA/");
#endif

	//noiseGenerator->SetSource(noiseSource);
	//noiseGenerator->SetOctaveCount(options.octaves);
	//noiseGenerator->SetLacunarity(1.5);
	unsigned int x = 0, y = 0;
	for (float i = 0; i < CHUNK_SIZE_PADDED; i++) {
		for (float j = 0; j < CHUNK_SIZE_PADDED; j++) {
			xs[x++] = j * NOISE_FACTOR * options.frequency;
			ys[y++] = i * NOISE_FACTOR * options.frequency;
		}
	}

}

std::vector<float> world_generation::getRawHeightsPadded(glm::vec2 chunkPos, unsigned int seed)
{
	std::vector<float> noiseOutput(CHUNK_AREA_PADDED);
	chunkPos -= 1;
	chunkPos *= NOISE_FACTOR * options.frequency;
	// deducts 1 to nullify the padding
	auto minMax = noiseGenerator->GenPositionArray2D(noiseOutput.data(), CHUNK_AREA_PADDED, xs.data(), ys.data(), chunkPos.x, chunkPos.y, seed);
	for (unsigned int i = 0; i < CHUNK_AREA_PADDED; i++)
	{
		noiseOutput[i] = noiseToHeight(noiseOutput[i], minMax);
	}
	return noiseOutput;
}

std::vector<float> world_generation::getRawHeights(glm::vec2 chunkPos, unsigned int seed)
{
	std::vector<float> noiseOutput(CHUNK_AREA);

	chunkPos *= NOISE_FACTOR * options.frequency;
	auto minMax = noiseGenerator->GenPositionArray2D(noiseOutput.data(), CHUNK_AREA, xs.data(), ys.data(), chunkPos.x, chunkPos.y, seed);
	for (unsigned int i = 0; i < CHUNK_AREA; i++)
	{
		noiseOutput[i] = noiseToHeight(noiseOutput[i], minMax);
	}
	return noiseOutput;
}

void world_generation::createHeightMap(glm::vec2 chunkPos, unsigned int seed, HeightMap& res, unsigned int biome) {
	std::vector<float> noiseOutput = std::move(getRawHeights(chunkPos, seed));

	for (unsigned int i = 0; i < CHUNK_AREA; i++)
	{
		res[i] = createColumn(noiseOutput[i]);
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
		// doesnt adds water on top of height
		// is a body of water
		unsigned int waterDepth = WATER_LEVEL - height;
		encoded.push(B_STONE, height - 3);
		encoded.push(B_SAND, 3);
		encoded.push(B_WATER, waterDepth);
	}
	else {		
		// is a land mass
		int h = randInt(1, 4);
		encoded.push(B_STONE, height - h);
		if (height > SNOW_LEVEL - h) {
			// is mountain
			encoded.push(B_SNOW, h);
		}
		else {
			encoded.push(B_DIRT, h - 1);
			encoded.push(B_GRASS, 1);
		}
	}

	/*if (height - 3 > 0) {
		encoded.push(B_STONE, height -= 3);
	}
	if (height <= WATER_LEVEL) {
		encoded.push(B_SAND, 3);
		encoded.push(B_WATER, 20 - height - 2);
	}
	else if (height < WATER_LEVEL + 3) {
		encoded.push(B_SAND, 3);
	}
	else if (height - 2 > 0) {
		encoded.push(B_DIRT, 2);
		encoded.push(B_GRASS, 1);
	}*/
	return encoded;
}

unsigned int world_generation::noiseToHeight(const float noise, const FastNoise::OutputMinMax& minMax)
{
	return (noise + 1.f) * .5f * 50;
}
