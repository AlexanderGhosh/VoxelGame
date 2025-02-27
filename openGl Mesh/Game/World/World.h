#pragma once
#include <future>
#include <unordered_set>
#include "../../GeomRendering/DrawableGeom.h"
#include "../../GreedyRendering/DrawableGreedy.h"
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Async/DynamicAsyncPool.h"

class World
{
public:
	World();
	~World();

	World(const World&) = delete;
	World& operator=(const World&) = delete;

	World(World&&) = delete;
	World& operator=(World&&) = delete;

	inline size_t getChunkCount() const {
		return chunks.size();
	}
	inline size_t getDrawCount() const {
		return geomDrawable.size();
	}

	inline void setSeed(unsigned int _seed) { _seed = _seed; }

	void render(Shader* shader);

	void generateTerrain();
	void setUpDrawable();

	void placeBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunk);
	void breakBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunkPos);

	// chunkPos - local unscaled
	const std::list<ChunkColumn*> getNeibours(const glm::vec2& chunkPos, bool includeSelf = false);

	// chunk pos - the currently occupiued chunk pos local unscaled
	// frustrumCenter - world scaled pos
	// radius - world scaled
	// performed on the main thread
	void tryStartGenerateChunks(const glm::vec2& chunkPos, const glm::vec3& fwd); 
	// performed on the main thread
	void tryFinishGenerateChunk();

	// if sucess is true returns a reference to the desired chunk
	// will not return partialy generated chunks (in the process of async generation)
	const ChunkColumn& getChunk(const glm::vec2& chunkPos, bool& success) const;
	// if sucess is true returns a reference to the desired chunk
	// will not return partialy generated chunks (in the process of async generation)
	ChunkColumn* getChunk(const glm::vec2& chunkPos, bool& success);

	// performs cellular automota on 0, 0 chunk
	void update();

	// blows up blocks centered at world pos with radius
	// is optimised for explosions dont use for breaking one block (CAN BE OPTIMISED BUT CURRENTLY NOT)
	// asumes the same chunk
	void explode(const glm::vec3& worldPos, float radius);

	// returns the block looking (error if none) updates hit Pos witht he pos of the block
	glm::vec3 lookingAt(const glm::vec3& ro, const glm::vec3& rd);

	DrawableGeom geomDrawable;
	DrawableGreedy greedyDrawable;
private:
	bool generated; // used to genrated the world async first
	DynamicAsyncPool<std::list<ChunkColumn>> pool;
	// set of all the chunk positions that are currently being created in the dyamic pool
	// local unscaled positions
	std::unordered_set<glm::vec2> positionsBeingGenerated;
	// spilts the set into n parts and calls 'generateNewChunks'
	void launchAsyncs(const std::unordered_set<glm::vec2>& allChunkPoss, const unsigned int n);

	unsigned int _seed;
	Chunks chunks;
	// only genertes chunk buffer data can be called async
	// returns a list of new chunk that have generated
	// performed on the any thread
	// the chunks returned are copyable because the buffer hasnt beign created yet
	std::list<ChunkColumn> generateNewChunks(const std::unordered_set < glm::vec2>& positions) const;
	// reutns a set of positions centered on the origin with a scale of 1
	const std::unordered_set<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;

	std::unordered_set<glm::vec2> positionsInFront(const glm::vec2& origin, const glm::vec3& front, int renderDist) const;
};