#pragma once
#include "../../../BlockStore.h"
#include "../../../GeomRendering/BufferGeom.h"
#include "../../../GeomRendering/GeomData.h"

class World;

class ChunkColumn
{
public:
	ChunkColumn();
	ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map);

	// runtime chunk generation
	// generates all chunk data needed for the the buffers
	void generateChunkData(glm::vec2 pos, unsigned int seed, const std::list<ChunkColumn*>& neibours);
	void setUpBuffer();
	void reallocBuffer();


	// inital world generation
	void populateBuffer(WorldMap& worldMap);

	const BufferGeom& getBuffer() const;
	BufferGeom* getBufferPtr();

	// doesnt check other chunks
	void addBlock(const glm::vec3& worldPos, const Block block);

	// doesnt check other chunks
	void removeBlock(const glm::vec3& worldPos, World* world);

	// returns the unscaled chunk position 2D
	const glm::vec2& getPosition() const;
	// returns the scaled chunk position 2D
	const glm::vec2 getWorldPosition() const;

	void save() const;
	void load(const glm::vec2& chunkPos);

	const std::vector<GeomData>& getMeshData() const;
private:

	// used for runtime genertion doesnt do any opengl funcs
	void populateBufferFromNeibours(const std::list<ChunkColumn*>& neibours, const BlockStore& blockStore);

	struct AddFaces {
		glm::vec3 worldPos;
		glm::vec3 offset;
		unsigned char face;
	};
	std::unordered_map<glm::vec3, Block> editedBlocks;
	std::vector<GeomData> bufferData;
	BufferGeom buffer;
	glm::vec2 position;

	unsigned int seed;

	const Block getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const;
	/// <summary>
	/// Gets the block from edited or simplex noise
	/// </summary>
	/// <param name="relativePos">ranges from (0-15)</param>
	/// <returns>thhe found block</returns>
	const Block getBlock(const glm::vec3& worldPos);
	const Block getBlock(const glm::vec3& worldPos, const std::vector<ChunkColumn*>& neibours, const BlockStore& bs);

	const Block getBlock(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const;
	const glm::vec3 getRelativePosition(glm::vec3 worldPos) const;
	const glm::vec3 getWorldPosition(glm::vec3 relativePos) const;

	void addFace(const AddFaces& data, bool realoc);

	static bool outOfRange(const glm::vec3& localPos);

	// returns the unscaled chunk position 3D
	glm::vec3 getWorldPos() const;
	// returns the position relative to this chunk 3D
	// doesnt check bounds
	glm::vec3 toLocal(const glm::vec3& p) const;
};