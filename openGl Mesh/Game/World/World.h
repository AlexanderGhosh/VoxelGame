#pragma once
#include <tuple>
#include <thread>
#include <vector>
#include <chrono>
#include "Chunk.h"
#include "constants.h"
#include "../Player/Player.h"
#include "../../Renders/chunkRender.h"

class World
{
public:
	World();
	World(Player& player);
	World(GLboolean gen);
	void renderChunksStatic(Camera c, glm::mat4 projection);
	void renderChunksDynamic();
	void processBlockUpdates();
	void setPlayerPosition(Player& player);
	Chunk* getChunkOccupied(glm::vec3 position);
	void cleanUp();
	std::vector<Chunk*> getChunks(); 
private:
	std::vector<std::pair<Chunk, GLboolean>> chunks;
	glm::vec3 playerPosition;
	Render::ChunkMeshRender render;
	void generateFlatChunks(std::vector<glm::vec3> chunkPositions);
	void getNewChunkPositions();
};