#pragma once
#include "../constants.h"
#include "../../../BlockStore.h"
#include "../../../GeomRendering/BufferGeom.h"

class ChunkColumn
{

public:

	ChunkColumn();
	ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map);

	// creators
	void populateBuffer(WorldMap& worldMap);

	// getters
	// const std::tuple<std::vector<Block_Count>*, unsigned int, ChunkColumn*> getHeightAt(glm::vec2 pos, bool safe, AdjacentMap_p& adjacent) const; // mabye could be optimised

	const BufferGeom& getBuffer() const;
private:
	BufferGeom buffer;
	glm::vec2 position;

	unsigned int seed;

	const Block getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const;

	const glm::vec2& getPosition() const;
	const Block getBlock(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const;
	const glm::vec3 getRelativePosition(glm::vec3 worldPos) const;
	const glm::vec3 getWorldPosition(glm::vec3 relativePos) const;
};