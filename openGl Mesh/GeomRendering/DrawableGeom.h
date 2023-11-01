#pragma once
#include <glm.hpp>
#include <list>
#include "../Helpers/Constants.h"
#include "DrawData.h"

class Camera;
class Shader;
class DrawData;

class DrawableGeom
{
public:
	DrawableGeom();
	~DrawableGeom();
	void render(Shader* shader) const;
	void setUp(const Chunks& chunks);

private:
	std::list<DrawData> data;
	void draw() const;
};

