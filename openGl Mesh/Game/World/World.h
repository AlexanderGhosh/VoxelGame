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

	void placeBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunk);
	void breakBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunkPos);

	// chunkPos - local unscaled
	const std::list<ChunkColumn*> getNeibours(const glm::vec2& chunkPos, bool includeSelf = false);

	void save() const;
	// chunk pos - the currently occupiued chunk pos local unscaled
	// frustrumCenter - world scaled pos
	// radius - world scaled
	void tryStartGenerateChunks(const glm::vec2& chunkPos, const glm::vec3& frustrumCenter, const float frustrumRadius); 
	void tryFinishGenerateChunk();

	const ChunkColumn& getChunk(const glm::vec2& chunkPos, bool& success) const;

private:
	bool generated; // used to genrated the world async first
	DynamicAsyncPool<std::unordered_set<glm::vec2>> pool;
	// set of all the chunk positions that are currently being created in the dyamic pool
	// local unscaled positions
	std::unordered_set<glm::vec2> positionsBeingGenerated;
	// spilts the set into n parts and calls 'generateNewChunks'
	void launchAsyncs(const std::unordered_set<glm::vec2>& allChunkPoss, const unsigned int n);

	unsigned int seed;
	Chunks chunks;

	DrawableGeom geomDrawable;
	// only genertes chunk buffer data can be called async
	// returns a list of chunk positions that it will generate
	std::unordered_set<glm::vec2> generateNewChunks(const std::unordered_set < glm::vec2>& positions);
	// reutns a set of positions centered on the origin with a scale of 1
	const std::unordered_set<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;
	void getNewChunkPositions(const glm::vec3 worldOrigin);
	void generateTerrain(const std::unordered_set<glm::vec2>& chunkPositions);
};