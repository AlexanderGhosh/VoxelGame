#pragma once
#include <list>
#include "DrawData.h"
#include "../Helpers/Constants.h"

class Shader;
class IGeomDrawable;

class DrawableGeom
{
public:
	DrawableGeom();
	~DrawableGeom();
	inline size_t size() const { return data.size(); }

	void render(Shader* shader) const;
	void setUp(Chunks& chunks);

	void add(const IGeomDrawable* elem);

	void remove(IGeomDrawable* elem);
	bool remove(const glm::vec2& chunkPos);

	// returns a reference to the draw data that has the buffer attached to it
	// this will work because the drawdata contains a pointer to the buffer
	DrawData* get(BufferGeom* buffer);

private:
	std::list<DrawData> data;
	void draw(Shader* shader) const;
};

