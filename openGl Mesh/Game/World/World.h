#pragma once
#include "../../GeomRendering/DrawableGeom.h"
#include "Chunks/ChunkColumn.h"

class World
{
public:
	World();
	World(bool gen, bool terrain = true, unsigned int seed = SEED);

	void render(Shader* shader) const;

	void setUpDrawable();

private:
	unsigned int seed;
	Chunks chunks;

	DrawableGeom geomDrawable;

	const std::vector<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;
	void getNewChunkPositions(bool flat);
	void generateTerrain(const std::vector<glm::vec2>& chunkPositions);
};