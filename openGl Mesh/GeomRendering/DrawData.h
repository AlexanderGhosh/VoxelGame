#pragma once

class BufferGeom;
class Texture;
struct DrawData {
	BufferGeom* buffer;
	Texture* texture;
	glm::vec3 drawOrigin;

	DrawData() : buffer(), texture(nullptr), drawOrigin() { }
	DrawData(BufferGeom* buffer, Texture* tex, glm::vec3 drawOrigin) : buffer(buffer), texture(tex), drawOrigin(drawOrigin){ }
};
