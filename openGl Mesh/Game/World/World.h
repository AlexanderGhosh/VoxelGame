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
	World(GLboolean gen, GLboolean terrain = 1, GLboolean isDynamic = 0, GLuint seed = CHUNK_SIZE*2);
	void render(Camera& c, glm::mat4 projection, glm::mat4 lightMatrix, GLuint depthMap);
	void render(glm::mat4 lightProjection, glm::mat4 lightView);
	void updatePlayerPos(glm::vec3 pos);

	void breakBlock(glm::vec3 pos, glm::vec3 front);
	void placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block);

	ChunkColumn* getChunkOccupied(glm::vec3 position);
	glm::vec2 getChunkOccPosition(glm::vec3 position);

	std::unordered_map<GLuint, FaceB_p>& getWorldMesh();
	std::vector<ChunkColumn*> getAdjacentChunks(glm::vec3 worldPosition);
	AdjacentMap getAdjacentMap(glm::vec3 worldPos, GLuint range = RENDER_DISTANCE);
	AdjacentMap_p getAdjacentMapPointers(glm::vec3 worldPos, GLuint range = RENDER_DISTANCE);
	
	void save();

	void advanceGeneration();

	GLboolean created;
	std::vector<GLuint> generationStack;
	GLuint seed;
	Chunks chunks2;
	std::vector<glm::vec2> centeredPositions(glm::vec2 origin, std::vector<glm::vec2> exclude, GLint renderDist = RENDER_DISTANCE);
	std::vector<glm::vec2> centeredPositions(glm::vec2 origin, std::vector<ChunkColumn>& exclude, GLint renderDist = RENDER_DISTANCE);

	Entity* getIntersectedEntity(EntityHander& handler, Ray ray);
	void genWorldMesh();
	DrawableGeom geomDrawable;
	std::unordered_map<GLuint, FaceB_p> worldMesh;
	Drawable drawable;
private:

	GLboolean isDynamic;
	GLboolean reDraw;
	glm::vec2 chunkOccupiedPosition;

	WorldMap worldMap;

	void getNewChunkPositions(GLboolean flat);

	void generateFlatChunks(std::vector<glm::vec2> chunkPositions);
	void generateTerrain(std::vector<glm::vec2>& chunkPositions);


	std::tuple<glm::vec3, FACES_NAMES> getIntersectedBlock(ChunkColumn*& chunkOcc, Ray ray);
};