#pragma once
#include <future>
#include <unordered_set>
#include "../../GeomRendering/DrawableGeom.h"
#include "Chunks/ChunkColumn.h"

class World
{
public:
	World();
	World(const glm::vec3 worldOrigin, unsigned int seed);

	void render(Shader* shader);

	void setUpDrawable();

	void placeBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);
	void breakBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front);

	const std::list<ChunkColumn*> getNeibours(const glm::vec2& chunkPos);

	void save() const;
	void tryStartGenerateChunks(const glm::vec2& chunkPos); 
	void tryFinishGenerateChunk();

private:

	std::future<void> chunkDataGenerated;
	bool chunkCreationInprogress;
	std::unordered_set<glm::vec2> generationPositions;

	unsigned int seed;
	Chunks chunks;

	DrawableGeom geomDrawable;

	void generateNewChunks(const glm::vec2& center);
	const std::unordered_set<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;
	void getNewChunkPositions(const glm::vec3 worldOrigin);
	void generateTerrain(const std::unordered_set<glm::vec2>& chunkPositions);

};