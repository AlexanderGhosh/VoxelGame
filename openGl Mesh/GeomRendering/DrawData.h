#pragma once

class BufferGeom;
class Texture;
struct DrawData {
	BufferGeom* buffer;
	Texture* texture;

	DrawData() : buffer(), texture(nullptr) { }
	DrawData(BufferGeom* buffer, Texture* tex) : buffer(buffer), texture(tex) { }
};
