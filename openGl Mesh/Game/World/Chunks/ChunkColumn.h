#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/hash.hpp"
#include <chrono>
#include "../constants.h"
#include "../../../BlockStore.h"
class ChunkColumn
{
private: // properties
	glm::vec2 position;
	// HeightMap heightMap;
	// std::unordered_map<glm::vec3, Blocks> blocksEdited;
	BlockStore* blockStore;
	std::unordered_map<GLuint, FaceB_p> mesh;
	GLubyte highest_natural_point;
	GLboolean isFlat;

	GLboolean breaking;

public: // functions

	GLubyte stage;
	GLboolean fromFile;

   // constructors
	ChunkColumn();
	ChunkColumn(glm::vec2 pos);
	ChunkColumn(std::string fileName);
	ChunkColumn(glm::vec2 pos, WorldMap* worldMap);

	// creators
	void createMesh(WorldMap* worldMap);
	void createMesh(AdjacentMap& adjacentCunks, HeightMap heighMap);
	void createMesh_flat(AdjacentMap& adjacentCunks);
	void addTrees();

	// getters
	std::unordered_map<GLuint, FaceB_p>& getMesh();
	glm::vec2 getPosition();
	std::pair<Blocks, ChunkColumn*> getBlock_ChunkPos(glm::vec3 worldPosition, AdjacentMap_p& allChunks);
	Blocks getBlock(glm::vec3 pos, GLboolean worldPos = 1);
	glm::vec3 getRelativePosition(glm::vec3 worldPos);
	glm::vec3 getWorldPosition(glm::vec3 relativePos);
	std::tuple<std::vector<Block_Count>*, GLuint, ChunkColumn*> getHeightAt(glm::vec2 pos, GLboolean safe, AdjacentMap_p& adjacent);
	std::string getFileName();
	BlockStore* getBlockStore();

	// setters
	void setBlockStore(BlockStore* bs);

	// opertations
	void editBlock(glm::vec3 pos, GLboolean worldPos, Blocks block, AdjacentMap_p& adjacentCunks);
	void save(std::string name, GLuint seed);
	void save(GLuint seed);
	void addToMesh(Face face);
	void removeFromMesh(Face face);

	// operators
	bool operator==(glm::vec2 pos);
	bool operator==(ChunkColumn chunk);

private: // functions
	// editors
	void addBlock(glm::vec3 position, GLboolean worldPos, Blocks block, WorldMap* worldMap);

	// getters
	Blocks getBlock(glm::vec3 pos, GLboolean worldPos, GLboolean safe, WorldMap* worldMap);
};
bool operator==(ChunkColumn* chunk, glm::vec2 pos);

struct Savable_ {
	GLfloat x, z;
	GLuint seed;
};
struct Tuple1 {
	GLubyte bufferType;
	GLubyte textureMap;
	GLuint size;
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

