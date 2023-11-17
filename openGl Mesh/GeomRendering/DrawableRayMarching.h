#pragma once
#include <glm.hpp>
#include <list>
#include "../Helpers/Constants.h"
#include "DrawDataRayMarch.h"

class Camera;
class Shader;

class DrawableRayMarching
{
public:
	DrawableRayMarching();
	~DrawableRayMarching();
	void render(Shader* shader) const;
	void setUp(Chunks& chunks);
	void add(ChunkColumn& chunks);
	void remove(const glm::vec2& chunkPos);
	void setPlane(const unsigned int vao);
private:
	unsigned int planeVAO;
	std::list<DrawDataRayMarch> data;
	void draw(Shader* shader) const;
};

