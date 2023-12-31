#pragma once
#include <ext/vector_float3.hpp>
#include <vector>
#include "../../Helpers/Constants.h"
#include "../../GeomRendering/BufferGeom.h"
#include "../../GeomRendering/IGeomDrawable.h"
#include "../../GreedyRendering/BufferGreedy.h"
#include "../../Block.h"

struct PointColourIndex {
	float x, y, z;
	Block block;
	PointColourIndex() = default;
};
struct GreedyColliderData {
	float _xSpan;
	float _ySpan;
	float _zSpan;
	glm::vec3 _center;

	GreedyColliderData() : _xSpan(0.01), _ySpan(0.01), _zSpan(0.01), _center() { }

	GreedyColliderData(const GreedyColliderData&) = default;
	GreedyColliderData& operator=(const GreedyColliderData&) = default;

	GreedyColliderData(GreedyColliderData&& other) noexcept : GreedyColliderData() {
		_xSpan = other._xSpan;
		_ySpan = other._ySpan;
		_zSpan = other._zSpan;
		_center = other._center;
	}
	GreedyColliderData& operator=(GreedyColliderData&& other) noexcept {
		_xSpan = other._xSpan;
		_ySpan = other._ySpan;
		_zSpan = other._zSpan;
		_center = other._center;
		return *this;
	}
};

namespace reactphysics3d {
	class RigidBody;
}

struct GreedyData;
class Shader;
class Block;

class VoxelMesh : public IGeomDrawable
{
	friend class VoxelModel;
public:
	VoxelMesh(const glm::vec3& relativePos, const std::vector<Block>& blocks, const std::unordered_map<glm::ivec3, std::vector<Block>>& neighbours);
#ifdef ALWAYS_USE_GREEDY_MESH
	BufferGreedy greedyBuffer;
#endif
private:
	glm::vec3 relativePos_;
	BufferGeom _buffer;
	reactphysics3d::RigidBody* rigidBody_;
	VoxelModel* parent;

	VoxelMesh();

	// asumes that all values of points are in the range 0-16

	DrawData getDrawData() const override;

	std::vector<GreedyData> greedyMesh(std::vector<Block>& cloud);
};

