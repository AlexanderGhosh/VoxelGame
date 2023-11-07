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

	void placeBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);
	void breakBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);

	const std::vector<ChunkColumn*> getNeibours(const glm::vec2& chunkPos);

	void save() const;
private:
	unsigned int seed;
	Chunks chunks;

	DrawableGeom geomDrawable;

	const std::vector<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;
	void getNewChunkPositions(bool flat);
	void generateTerrain(const std::vector<glm::vec2>& chunkPositions);

};