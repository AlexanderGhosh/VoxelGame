#pragma once
#include <ext/vector_float3.hpp>
#include "../Helpers/Constants.h"

class BufferGeom;
class Texture;
struct DrawData {
	enum Type { CHUNK, MODEL, NA } _type;
	BufferGeom* _buffer;
	// the local position of the chunk unscaled
	glm::vec3 _drawOrigin;
	float _voxelSize;

	DrawData() : _buffer(), _drawOrigin(), _type(NA), _voxelSize(VOXEL_SIZE) { }
	DrawData(BufferGeom* buffer, glm::vec3 drawOrigin, Type type) : _buffer(buffer), _drawOrigin(drawOrigin), _type(type), _voxelSize(VOXEL_SIZE) { }

	bool operator==(const DrawData& other) const {
		return _type == other._type &&
			_buffer == other._buffer &&
			_drawOrigin == other._drawOrigin;
	}
	bool operator!=(const DrawData& other) const {
		return *this == other;
	}
};
