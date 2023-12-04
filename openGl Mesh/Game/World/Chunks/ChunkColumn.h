#pragma once
#include "../../../BlockStore.h"
#include "../../../GeomRendering/BufferGeom.h"
#include "../../../GeomRendering/GeomData.h"
#include "../../../GreedyRendering/GreedyData.h"
#include "../../../GreedyRendering/BufferGreedy.h"

class World;

// all operations are unscaled unless otherwise stated
class ChunkColumn
{
	friend class World;
public:
	ChunkColumn();
	ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map);

	// runtime chunk generation
	// generates all chunk data needed for the the buffers
	void generateChunkData(glm::vec2 pos, unsigned int seed, const std::list<ChunkColumn*>& neibours);
	void setUpBuffer();
	void reallocBuffer();


	// inital world generation
	// NOT UPDATED
	void populateBuffer(WorldMap& worldMap);

	const BufferGeom& getBuffer() const;
	BufferGeom* getBufferPtr();

	// doesnt check other chunks
	void addBlock(const glm::vec3& worldPos, const Block block);

	// doesnt check other chunks
	void removeBlock(const glm::vec3& worldPos, World* world);

	// returns the unscaled chunk position 2D
	const glm::vec2& getPosition2D() const;
	// returns the unscaled chunk position 3D
	const glm::vec3 getPosition3D() const;
	// returns the world chunk position 2D
	const glm::vec2 getWorldPosition2D() const;
	// returns the world chunk position 3D
	const glm::vec3 getWorldPosition3D() const;

	void save() const;
	void load(const glm::vec2& chunkPos);

	// x and z are local coords
	// returns the height of the column as if it had just being newly generated (doesnt check edited blocks)
	unsigned int getHeight(const float x, const float z) const;

	const std::vector<GeomData>& getMeshData() const;

	// returns the current state of the mesh in grid format
	std::array<BlockDetails, CHUNK_AREA* WORLD_HEIGHT> getBlocksGrid();

	// GREEDY MESH STUFF
	BufferGreedy* getGreedyPtr() {
		return &greedyBuffer;
	};
	void setUpGreedyBuffer();

private:
	// GREEDY MESH STUFF
	std::vector<GreedyData> greedyBufferData;
	BufferGreedy greedyBuffer;

	void createRunPX(Block currentBlock, int& x, int y, int z, const BlockStore& blockStore);
	void createRunPZ(Block currentBlock, int& x, int y, int z, const BlockStore& blockStore);
	void createRunNZ(Block currentBlock, int& x, int y, int z, const BlockStore& blockStore);
	//

	// used for runtime genertion doesnt do any opengl funcs
	void populateBufferFromNeibours(const std::list<ChunkColumn*>& neibours, const BlockStore& blockStore);
	// greedy mesh
	void greedyMesh(const std::list<ChunkColumn*>& neibours, const BlockStore& blockStore);

	struct AddFaces {
		glm::vec3 worldPos;
		glm::vec3 offset;
		unsigned char face;
	};
	std::unordered_map<glm::vec3, Block> editedBlocks;
	std::vector<GeomData> bufferData;
	BufferGeom buffer;
	// unscaled local pos 
	glm::vec2 position;

	unsigned int seed;

	// Gets the block from edited or simplex noise in the world pos
	// all getBlock s resovel to this
	const Block getBlock(const glm::vec3& worldPos);
	// returns the block found at the position will check the neibour chunks (sourced from world map) if out of bounds
	const Block getBlock_WorldMap(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const;
	// returns the block found at in the provided block store
	// does no bounds checing
	const Block getBlock_BlockStore(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const;

	void addFace(const AddFaces& data, bool realoc);

	static bool outOfRange(const glm::vec3& localPos);

	// returns the position relative to this chunk 3D
	// doesnt check bounds
	glm::vec3 toLocal(const glm::vec3& p) const;
	// returns the position relative to the world
	// doesnt check bounds
	glm::vec3 toWorld(const glm::vec3& p) const;
};