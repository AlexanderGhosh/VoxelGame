#pragma once
#include "../../../BlockStore.h"
#include "../../../GeomRendering/BufferGeom.h"
#include "../../../GeomRendering/GeomData.h"

class ChunkColumn
{
public:
	ChunkColumn();
	ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map);

	// creators
	void populateBuffer(WorldMap& worldMap);
	const BufferGeom& getBuffer() const;
	BufferGeom* getBufferPtr();

	void addBlock(const glm::vec3& worldPos, const Block block);
	void removeBlock(const glm::vec3& worldPos);

	const glm::vec2& getPosition() const;
private:
	std::vector<GeomData> bufferData;
	BufferGeom buffer;
	glm::vec2 position;

	unsigned int seed;

	const Block getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const;

	const Block getBlock(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const;
	const glm::vec3 getRelativePosition(glm::vec3 worldPos) const;
	const glm::vec3 getWorldPosition(glm::vec3 relativePos) const;
};