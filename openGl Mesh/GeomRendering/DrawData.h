#pragma once
#include <ext/vector_float3.hpp>

class BufferGeom;
class Texture;
struct DrawData {
	enum Type { CHUNK, MODEL, NA } type_;
	BufferGeom* buffer_;
	// the local position of the chunk unscaled
	glm::vec3 drawOrigin_;

	DrawData() : buffer_(), drawOrigin_(), type_(NA) { }
	DrawData(BufferGeom* buffer, glm::vec3 drawOrigin, Type type) : buffer_(buffer), drawOrigin_(drawOrigin), type_(type) { }

	bool operator==(const DrawData& other) const {
		return type_ == other.type_ &&
			buffer_ == other.buffer_ &&
			drawOrigin_ == other.drawOrigin_;
	}
	bool operator!=(const DrawData& other) const {
		return *this == other;
	}
};
