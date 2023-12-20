#pragma once
#include <glm.hpp>
#include <list>
#include "../Helpers/Constants.h"
#include "GreedyDrawData.h"

class Camera;
class Shader;

class DrawableGreedy
{
public:
	DrawableGreedy();
	~DrawableGreedy();
	void render(Shader* shader) const;
	void setUp(Chunks& chunks);
	void add(ChunkColumn& chunks);
	void remove(const glm::vec2& chunkPos);

	void add(BufferGreedy* buffer, glm::vec3 pos);
private:
	std::list<GreedyDrawData> data;
	void draw(Shader* shader) const;
};

