#pragma once
#include "BufferGeom.h"

class Texture;
struct DrawData {
	BufferGeom buffer;
	Texture* texture;

	DrawData() : buffer(), texture(nullptr) { }
	DrawData(const BufferGeom& buffer, Texture* tex) : buffer(buffer), texture(tex) { }
};
