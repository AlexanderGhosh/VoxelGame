#pragma once
#include <tuple>
#include <thread>
#include <vector>
#include <chrono>
#include "Chunk.h"
#include "constants.h"
#include "../../Renders/chunkRender.h"
#include "../../Drawable.h"

class World
{
public:
	World();
	World(GLboolean gen, GLboolean flat = true);
	void renderChunksStatic(Camera c, glm::mat4 projection);
	void renderChunksDynamic();
	void processBlockUpdates();
	void cleanUp();
	std::vector<Chunk*> getChunks();
	Chunk& getOccupiedChunk(glm::ivec3 occPosition);
	std::vector<Face*>& getWorldMesh();
private:
	std::vector<std::pair<Chunk, GLboolean>> chunks;
	Render::ChunkMeshRender render;
	std::vector<Face*> worldMesh;
	Drawable drawable;
	void generateFlatChunks(std::vector<glm::vec3> chunkPositions);
	void generateTerrain(std::vector<glm::vec3> chunkPositions);
	void getNewChunkPositions(GLboolean flat);
	void genWorldMesh();
};