#pragma once
#include <glm.hpp>

struct BufferGreedy;
struct GreedyDrawData {
	BufferGreedy* _buffer;

	glm::vec3 _drawOrigin;

	GreedyDrawData() : _buffer(nullptr), _drawOrigin() { }
	GreedyDrawData(BufferGreedy* buffer_, glm::vec3 drawOrigin_) : _buffer(buffer_), _drawOrigin(drawOrigin_) { }
};