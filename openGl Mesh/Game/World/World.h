#pragma once
#include <tuple>
#include <thread>
#include <vector>
#include <chrono>
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
	World(GLboolean gen, GLboolean flat = 1, GLboolean isDynamic = 0);
	void processBlockUpdates();
	void render(Camera& c, glm::mat4 projection);
	std::vector<chunk_column*> getChunks();
	std::vector<Face*>& getWorldMesh();
	void updatePlayerPos(glm::vec3* pos);

	void createChunk(ChunkPosition position);
	void breakBlock(glm::vec3 pos, glm::vec3 front);
private:
	std::vector<std::pair<chunk_column, GLboolean>> chunks;
	std::vector<Face*> worldMesh;
	Drawable drawable;
	GLboolean isDynamic;
	GLboolean reDraw;
	glm::vec3* playerPos;
	void generateFlatChunks(std::vector<glm::vec2> chunkPositions);
	void generateTerrain(std::vector<glm::vec2> chunkPositions);
	void getNewChunkPositions(GLboolean flat);
	std::vector<glm::vec2> getNewChunkPositions(glm::vec2 origin, GLint renderDist = INITALL_VIEW);
	void genWorldMesh();
	std::vector<Face*> genWorldMesh(std::vector<Chunk*> chunks);
	void renderChunksStatic(Camera& c, glm::mat4 projection);
	void renderChunksDynamic(Camera& c, glm::mat4 projection);

	GLubyte& getBlock(glm::ivec3 blockPos, chunk_column*& chunk_); // uses absolute block position
};