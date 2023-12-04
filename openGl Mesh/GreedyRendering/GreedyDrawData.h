#pragma once
#include <glm.hpp>

struct BufferGreedy;
struct GreedyDrawData {
	BufferGreedy* _buffer;

	glm::vec3 _drawOrigin;

	GreedyDrawData() : _buffer(nullptr), _drawOrigin() { }
	GreedyDrawData(BufferGreedy* buffer, glm::vec3 drawOrigin) : _buffer(buffer), _drawOrigin(drawOrigin) { }
};