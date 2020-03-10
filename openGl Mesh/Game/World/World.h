#pragma once
#include <tuple>
#include <thread>
#include <vector>
#include <chrono>
#include <gtx/hash.hpp>
#include <gtc/noise.hpp>
#include "Chunks/Chunk.h"
#include "constants.h"
#include "../../Renders/chunkRender.h"
#include "../../Drawable.h"
#include "Chunks/chunk_column.h"

class World
{
public:
	World();
	World(GLboolean gen, GLboolean terrain = 1, GLboolean isDynamic = 0, GLuint seed = CHUNK_SIZE*2);
	void render(Camera& c, glm::mat4 projection);
	std::vector<chunk_column*> getChunks();
	std::vector<Face*>& getWorldMesh();
	void updatePlayerPos(glm::vec3 pos);

	void createChunk(ChunkPosition position, GLboolean updateMesh = 1);
	void removeChunk(ChunkPosition position);

	void breakBlock(glm::vec3 pos, glm::vec3 front);
	void placeBlock(glm::vec3 pos, glm::vec3 front);

	Chunk* getSubChunk(glm::ivec3 pos);
	std::vector<Chunk*> getSubChunk(glm::ivec3 pos, GLboolean surrounding);
	std::vector<Chunk*> getSubChunks();
	chunk_column* getChunkOccupied(glm::vec3 position);

private:
	GLuint seed;
	std::vector<chunk_column> chunks;
	std::vector<Face*> worldMesh;
	Drawable drawable;
	GLboolean isDynamic;
	GLboolean reDraw;
	ChunkPosition prevChunkPos;

	void generateFlatChunks(std::vector<glm::vec2> chunkPositions);
	void generateTerrain(std::vector<glm::vec2> chunkPositions);
	void getNewChunkPositions(GLboolean flat);
	std::vector<glm::vec2> getNewChunkPositions(glm::vec2 origin, GLint renderDist = INITALL_VIEW);
	void genWorldMesh(GLboolean gen = 0);
	std::vector<Face*> genWorldMesh(std::vector<Chunk*> chunks);
	void renderChunksStatic(Camera& c, glm::mat4 projection);
	void renderChunksDynamic(Camera& c, glm::mat4 projection);

	Blocks& getBlock(glm::ivec3 blockPos, chunk_column*& chunk_); // uses absolute block position

	std::vector<chunk_column> createChunks(std::vector<ChunkPosition> positions, GLboolean lazyLoading, GLboolean rectifyExisting);
};