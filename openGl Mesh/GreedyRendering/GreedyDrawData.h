#pragma once
#include <glm.hpp>

struct BufferGreedy;
struct GreedyDrawData {
	BufferGreedy* _buffer;

	glm::vec3 _drawOrigin;

	GreedyDrawData() : _buffer(nullptr), _drawOrigin() { }
	GreedyDrawData(BufferGreedy* _buffer, glm::vec3 _drawOrigin) : _buffer(_buffer), _drawOrigin(_drawOrigin) { }
};