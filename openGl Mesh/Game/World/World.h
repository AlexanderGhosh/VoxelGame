#pragma once
#include <future>
#include <unordered_set>
#include "../../GeomRendering/DrawableGeom.h"
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Async/DynamicAsyncPool.h"

class World
{
public:
	World();
	World(const glm::vec3 worldOrigin, unsigned int seed);

	void render(Shader* shader);

	void setUpDrawable();

	void placeBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);
	void breakBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);

	const std::list<ChunkColumn*> getNeibours(const glm::vec2& chunkPos);

	void save() const;
	void tryStartGenerateChunks(const glm::vec2& chunkPos, const glm::vec3& frustrumCenter, const float frustrumRadius); 
	void tryFinishGenerateChunk();

private:
	DynamicAsyncPool<std::unordered_set<glm::vec2>> pool;
	// set of all the chunk positions that are currently being created in the dyamic pool
	std::unordered_set<glm::vec2> positionsBeingGenerated;
	// spilts the set into n parts and calls 'generateNewChunks'
	void launchAsyncs(const std::unordered_set<glm::vec2>& allChunkPoss, const unsigned int n);

	unsigned int seed;
	Chunks chunks;

	DrawableGeom geomDrawable;
	// only genertes chunk buffer data can be called async
	// returns a list of chunk positions that it will generate
	std::unordered_set<glm::vec2> generateNewChunks(const std::unordered_set < glm::vec2>& positions);
	const std::unordered_set<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;
	void getNewChunkPositions(const glm::vec3 worldOrigin);
	void generateTerrain(const std::unordered_set<glm::vec2>& chunkPositions);

};