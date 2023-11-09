#pragma once
#include "../../GeomRendering/DrawableGeom.h"
#include "Chunks/ChunkColumn.h"
#include <future>

class World
{
public:
	World();
	World(bool gen, bool terrain = true, unsigned int seed = SEED);

	void render(Shader* shader);

	void setUpDrawable();

	void placeBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);
	void breakBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);

	const std::list<ChunkColumn*> getNeibours(const glm::vec2& chunkPos);

	void save() const;
	void startGenerateChunk(const glm::vec2& chunkPos);
	void tryFinishGenerateChunk(const glm::vec2& chunkPos);
private:
	std::future<void> chunkDataGenerated;
	bool chunkCreationInprogress;
	unsigned int seed;
	Chunks chunks;

	DrawableGeom geomDrawable;

	const std::vector<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;
	void getNewChunkPositions(bool flat);
	void generateTerrain(const std::vector<glm::vec2>& chunkPositions);

};