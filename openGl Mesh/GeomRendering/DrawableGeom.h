#pragma once
#include <glm.hpp>
#include <list>
#include "../Helpers/Constants.h"
#include "DrawData.h"

class Camera;
class Shader;
class DrawData;
class IGeomDrawable;

class DrawableGeom
{
public:
	DrawableGeom();
	~DrawableGeom();
	void render(Shader* shader) const;
	void setUp(Chunks& chunks);

	void add(const IGeomDrawable* elem);

	void remove(IGeomDrawable* elem);
	void remove(const glm::vec2& chunkPos);

private:
	std::list<DrawData> data;
	void draw(Shader* shader) const;
};

