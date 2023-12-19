#pragma once
#include <ext/vector_float3.hpp>
#include <ext/vector_int3.hpp>
#include <vector>
#include "../../GeomRendering/BufferGeom.h"
#include "../../GeomRendering/IGeomDrawable.h"

struct PointColourIndex {
	float x, y, z;
	unsigned int idx;
	PointColourIndex() = default;
};

namespace reactphysics3d {
	class RigidBody;
}

class Shader;

class VoxelMesh : public IGeomDrawable
{
	friend class VoxelModel_Static;
public:
	VoxelMesh(const glm::vec3& relativePos, std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours, bool hasCollider);
private:
	glm::vec3 relativePos_;
	BufferGeom buffer_;
	reactphysics3d::RigidBody* rigidBody_;
	VoxelModel_Static* parent;

	VoxelMesh();

	// asumes that all values of points are in the range 0-16

	DrawData getDrawData() const override;
};

