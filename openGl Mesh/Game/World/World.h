#pragma once
#include <future>
#include <unordered_set>
#include "../../GeomRendering/DrawableRayMarching.h"
#include "Chunks/ChunkColumn.h"

class World
{
public:
	World();
	World(unsigned int seed);

	void render(Shader* shader);

	void setUpDrawable(const unsigned int planeVAO);

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

	DrawableRayMarching drawable;

	void generateNewChunks(const glm::vec2& center);
	const std::unordered_set<glm::vec2> centeredPositions(const glm::vec2& origin, int renderDist) const;
	void getNewChunkPositions();
	void generateTerrain(const std::unordered_set<glm::vec2>& chunkPositions);

};