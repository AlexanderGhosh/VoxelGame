#pragma once
#include "constants.h"
#include "../../GeomRendering/DrawableGeom.h"
#include "Chunks/ChunkColumn.h"

class World
{
public:
	World();
	World(bool gen, bool terrain = true, unsigned int seed = SEED);

	void render(Camera& c, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap, Shader* shader) const;

	void setUpDrawable();

private:
	unsigned int seed;
	Chunks chunks;

	DrawableGeom geomDrawable;

	const std::vector<glm::vec2> centeredPositions(glm::vec2 origin, const std::vector<glm::vec2>& exclude, int renderDist = RENDER_DISTANCE) const;
	void getNewChunkPositions(bool flat);
	void generateTerrain(const std::vector<glm::vec2>& chunkPositions);
};