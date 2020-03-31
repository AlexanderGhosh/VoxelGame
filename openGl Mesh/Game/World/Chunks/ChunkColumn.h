#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/hash.hpp"
#include <chrono>
#include "../constants.h"
#include "../world_generation.h"
class ChunkColumn
{
private: // properties
	glm::vec2 position;
	std::unordered_map<glm::vec3, Blocks> blocksEdited;
	std::unordered_map<GLuint, FaceB_p> mesh;
	HeightMap heightMap;
	GLubyte highest_natural_point;
	GLboolean isFlat;

	GLboolean breaking;

public: // functions

	GLubyte stage;


   // constructors
	ChunkColumn();
	ChunkColumn(glm::vec2 pos);
	ChunkColumn(std::string fileName);
	ChunkColumn(glm::vec2 pos, HeightMap heightMap);

	// creators
	void createMesh(AdjacentMap& adjacentCunks);
	void createMesh(AdjacentMap& adjacentCunks, HeightMap heighMap);
	void createMesh_flat(AdjacentMap& adjacentCunks);
	void addTrees(AdjacentMap_p& adjacent);

	// getters
	std::unordered_map<GLuint, FaceB_p>& getMesh();
	glm::vec2 getPosition();
	std::pair<Blocks, ChunkColumn*> getBlock_ChunkPos(glm::vec3 worldPosition, AdjacentMap_p& allChunks);
	Blocks getBlock(glm::vec3 pos, GLboolean worldPos = 1);
	glm::vec3 getRelativePosition(glm::vec3 worldPos);
	glm::vec3 getWorldPosition(glm::vec3 relativePos);
	std::tuple<std::vector<Block_Count>*, GLuint, ChunkColumn*> getHeightAt(glm::vec2 pos, GLboolean safe, AdjacentMap_p& adjacent);

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
	void addBlock(glm::vec3 position, GLboolean worldPos, Blocks block, AdjacentMap& adjacentCunks);

	// getters
	Blocks getBlock(glm::vec3 pos, GLboolean worldPos, GLboolean safe, AdjacentMap& ajacentChunks);
	std::string getFileName();
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

