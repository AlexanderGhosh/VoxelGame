#pragma once
#include <tuple>
#include <vector>
#include <chrono>
#include <gtc/noise.hpp>

#include "constants.h"
#include "../../Drawable.h"
#include "../../Buffer.h"
#include "../../Ray.h"
#include "Chunks/ChunkColumn.h"
#include "../../EntityHander.h"
#include "../../BlockStore.h"

#include "../../GeomRendering/DrawableGeom.h"

class World
{
public:
	World();
	World(bool gen, bool terrain = true, bool isDynamic = false, unsigned int seed = SEED);
	void render(Camera& c, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap);

	void breakBlock(glm::vec3 pos, glm::vec3 front);
	void placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block);

	ChunkColumn* getChunkOccupied(glm::vec3 position);

	std::vector<ChunkColumn*> getAdjacentChunks(glm::vec3 worldPosition);
	AdjacentMap getAdjacentMap(glm::vec3 worldPos, unsigned int range = RENDER_DISTANCE);
	AdjacentMap_p getAdjacentMapPointers(glm::vec3 worldPos, unsigned int range = RENDER_DISTANCE);
	
	std::vector<glm::vec2> centeredPositions(glm::vec2 origin, std::vector<glm::vec2> exclude, int renderDist = RENDER_DISTANCE);
	std::vector<glm::vec2> centeredPositions(glm::vec2 origin, std::vector<ChunkColumn>& exclude, int renderDist = RENDER_DISTANCE);

	Entity* getIntersectedEntity(EntityHander& handler, Ray ray);

	void setUpDrawable() {
		geomDrawable.setUp(chunks2);
	}
private:
	bool created;
	unsigned int seed;
	Chunks chunks2;

	DrawableGeom geomDrawable;
	bool isDynamic;
	bool reDraw;

	WorldMap worldMap;

	void getNewChunkPositions(bool flat);

	void generateFlatChunks(std::vector<glm::vec2> chunkPositions);
	void generateTerrain(std::vector<glm::vec2>& chunkPositions);


	std::tuple<glm::vec3, FACES_NAMES> getIntersectedBlock(ChunkColumn*& chunkOcc, Ray ray);
};