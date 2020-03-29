#pragma once
#include <tuple>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <gtc/noise.hpp>

#include "constants.h"
#include "../../Drawable.h"
#include "../../Ray.h"
#include "Chunks/ChunkColumn.h"

class World
{
public:
	World();
	World(GLboolean gen, GLboolean terrain = 1, GLboolean isDynamic = 0, GLuint seed = CHUNK_SIZE*2);
	void render(Camera& c, glm::mat4 projection);
	void updatePlayerPos(glm::vec3 pos);

	void breakBlock(glm::vec3 pos, glm::vec3 front);
	void placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block);

	ChunkColumn* getChunkOccupied(glm::vec3 position); 
	std::unordered_map<GLuint, FaceB_p>& getWorldMesh();
	std::vector<ChunkColumn*> getAdjacentChunks(glm::vec3 worldPosition);
	AdjacentMap getAdjacentMap(glm::vec3 worldPos, GLuint range = RENDER_DISTANCE);
	AdjacentMap_p getAdjacentMapPointers(glm::vec3 worldPos, GLuint range = RENDER_DISTANCE);
	
	void save();

	void advanceGeneration();


	std::vector<ChunkColumn*> generationStack;
	Chunks activeBuffer;
	GLuint seed;
	Chunks chunks2;
	std::vector<glm::vec2> centeredPositions(glm::vec2 origin, std::vector<glm::vec2> exclude, GLint renderDist = RENDER_DISTANCE);
private:
	std::unordered_map<GLuint, FaceB_p> worldMesh;
	Drawable drawable;
	GLboolean isDynamic;
	GLboolean reDraw;
	glm::vec2 chunkOccupiedPosition;


	void getNewChunkPositions(GLboolean flat);
	std::vector<glm::vec2> getNewChunkPositions(glm::vec2 origin, GLint renderDist = INITALL_VIEW);

	void generateFlatChunks(std::vector<glm::vec2> chunkPositions);
	void generateTerrain(std::vector<glm::vec2> chunkPositions, AdjacentMap adjacent);

	void genWorldMesh();

	void renderChunksStatic(Camera& c, glm::mat4 projection);
	void renderChunksDynamic(Camera& c, glm::mat4 projection);

	std::tuple<glm::vec3, FACES_NAMES> getIntersected(ChunkColumn*& chunkOcc, glm::vec2 in_chunkPos, Ray ray);

	std::vector<ChunkColumn> createChunks(std::vector<glm::vec2> positions, std::vector<glm::vec2> activeChunks,
		std::vector<glm::vec2> lazyChunks, GLboolean rectifyForExisting, GLboolean checkLazy, std::vector<ChunkColumn>* subChunks);
};