#pragma once
#include <ext/vector_float3.hpp>

class NoiseBuffer;
struct NoiseRenderData {
	NoiseBuffer* buffer_;
	glm::vec3 chunkPos_;

	NoiseRenderData() = default;
	NoiseRenderData(NoiseBuffer* buffer, const glm::vec3& chunkPos) : buffer_(buffer), chunkPos_(chunkPos) { }
};