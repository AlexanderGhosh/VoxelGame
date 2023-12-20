#pragma once
#include <ext/vector_float3.hpp>
#include <vector>
#include "../../Helpers/Constants.h"
#include "../../GeomRendering/BufferGeom.h"
#include "../../GeomRendering/IGeomDrawable.h"
#include "../../GreedyRendering/BufferGreedy.h"

struct PointColourIndex {
	float x, y, z;
	unsigned int idx;
	PointColourIndex() = default;
};
struct GreedyColliderData {
	float _xSpan;
	float _ySpan;
	float _zSpan;
	glm::vec3 _center;
	glm::vec3 _normal;

	unsigned int _materialIdx;

	GreedyColliderData() : _xSpan(), _ySpan(), _zSpan(), _center(), _normal(), _materialIdx() { }

	GreedyColliderData(const GreedyColliderData&) = default;
	GreedyColliderData& operator=(const GreedyColliderData&) = default;

	GreedyColliderData(GreedyColliderData&& other) noexcept : GreedyColliderData() {
		_xSpan = other._xSpan;
		_ySpan = other._ySpan;
		_zSpan = other._zSpan;
		_center = other._center;
		_normal = other._center;
		_materialIdx = other._materialIdx;
	}
	GreedyColliderData& operator=(GreedyColliderData&& other) noexcept {
		_xSpan = other._xSpan;
		_ySpan = other._ySpan;
		_zSpan = other._zSpan;
		_center = other._center;
		_normal = other._center;
		_materialIdx = other._materialIdx;
		return *this;
	}
};

namespace reactphysics3d {
	class RigidBody;
}

struct GreedyData;
class Shader;
enum class Block : unsigned char;

class VoxelMesh : public IGeomDrawable
{
	friend class VoxelModel_Static;
public:
	VoxelMesh(const glm::vec3& relativePos, std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours, bool hasCollider);
#ifdef ALWAYS_USE_GREEDY_MESH
	BufferGreedy greedyBuffer;
#endif
private:
	glm::vec3 relativePos_;
	BufferGeom buffer_;
	reactphysics3d::RigidBody* rigidBody_;
	VoxelModel_Static* parent;

	VoxelMesh();

	// asumes that all values of points are in the range 0-16

	DrawData getDrawData() const override;

	std::vector<GreedyData> greedyMesh(std::vector<Block>& cloud);
};

