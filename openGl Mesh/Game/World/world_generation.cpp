#include "world_generation.h"
#include "../../Helpers/Functions.h"
#include "../../Helpers/BlocksEncoded.h"


FastNoise::SmartNode<FastNoise::Simplex> world_generation::noiseSource;
FastNoise::SmartNode<FastNoise::FractalFBm> world_generation::noiseGenerator;
std::vector<float> world_generation::xs(CHUNK_AREA), world_generation::ys(CHUNK_AREA);
glm::ivec2 world_generation::treeCooldown = glm::vec2(4);

void world_generation::setUp()
{
	noiseSource = FastNoise::New<FastNoise::Simplex>();
	noiseGenerator = FastNoise::New<FastNoise::FractalFBm>();

	noiseGenerator->SetSource(noiseSource);
	noiseGenerator->SetOctaveCount(5);
	unsigned int x = 0, y = 0;
	for (float i = 0; i < CHUNK_SIZE; i++) {
		for (float j = 0; j < CHUNK_SIZE; j++) {
			xs[x++] = j * NOISE_FACTOR;
			ys[y++] = i * NOISE_FACTOR;
		}
	}
}
void world_generation::createHeightMap(glm::vec2 chunkPos, unsigned int seed, HeightMap& res, unsigned int biome) {
	std::vector<float> noiseOutput(CHUNK_AREA);

	chunkPos *= NOISE_FACTOR;
	auto minMax = noiseGenerator->GenPositionArray2D(noiseOutput.data(), CHUNK_AREA, xs.data(), ys.data(), chunkPos.x, chunkPos.y, seed);

	for (unsigned int i = 0; i < CHUNK_AREA; i++)
	{
		unsigned int height = noiseToHeight(noiseOutput[i], minMax);
		res[i] = createColumn(height);
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

unsigned int world_generation::heightOfColumn(glm::vec2 worldPos, const unsigned int seed)
{
	worldPos *= NOISE_FACTOR;
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

unsigned int world_generation::noiseToHeight(const float noise, const FastNoise::OutputMinMax& minMax)
{
	return (noise + 1.f) * .5f * 30;
}
