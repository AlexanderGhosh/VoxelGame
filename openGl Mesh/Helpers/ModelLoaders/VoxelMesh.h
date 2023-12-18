#pragma once
#include <ext/vector_float3.hpp>
#include <ext/vector_int3.hpp>
#include <vector>
#include "../../GeomRendering/BufferGeom.h"

struct PointColourIndex {
	float x, y, z;
	unsigned int idx;
	PointColourIndex() = default;
};

class Shader;
class VoxelMesh
{
	friend class VoxelModel_Static;
public:
	VoxelMesh(const glm::vec3& relativePos, std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours);
private:
	glm::vec3 relativePos_;
	BufferGeom buffer_;
	VoxelMesh();

	// asumes that all values of points are in the range 0-16

	void render(const Shader& shader, const glm::vec3& parentPos) const;
};

