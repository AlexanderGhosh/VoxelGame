#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/hash.hpp"
#include "../constants.h"
#include "../../../BlockStore.h"
#include "../../../GeomRendering/BufferGeom.h"

class ChunkColumn
{
private: // properties
	glm::vec2 position;
	// HeightMap heightMap;
	// std::unordered_map<glm::vec3, Blocks> blocksEdited;
	BlockStore blockStore;
	BufferGeom buffer;

public: // functions

   // constructors
	ChunkColumn();
	ChunkColumn(glm::vec2 pos, unsigned int seed);

	// creators
	void createMeshNew(WorldMap* worldMap);
	void addTrees();

	// getters
	const BufferGeom& getBuffer() const;
	const BlockStore& getBlockStore() const;
	const glm::vec2& getPosition() const ;
	Blocks getBlock(glm::vec3 pos, bool worldPos = 1, bool checkEdited = 1);
	glm::vec3 getRelativePosition(glm::vec3 worldPos);
	glm::vec3 getWorldPosition(glm::vec3 relativePos);
	std::tuple<std::vector<Block_Count>*, unsigned int, ChunkColumn*> getHeightAt(glm::vec2 pos, bool safe, AdjacentMap_p& adjacent); // mabye could be optimised

	// setters
	void editBlock(glm::vec3 pos, bool worldPos, Blocks block, WorldMap* worldmap, AdjacentMap_p& allChunks);

private: // functions
	// editors
	void addBlock(glm::vec3 position, bool worldPos, Blocks block, WorldMap* worldMap);

	// getters
	Blocks getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap* worldMap);
};
struct Tuple1 {
	GLubyte bufferType;
	GLubyte textureMap;
	unsigned int size;
};
struct Tuple_ {
	Tuple1 data;
	std::vector<glm::mat4> models;
	FaceB_p toFace();
};
struct BlockData {
	glm::vec3 pos;
	GLubyte block;
	std::pair<glm::vec3, Blocks> toBlockD();
};

