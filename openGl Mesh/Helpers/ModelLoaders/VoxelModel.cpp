#include "VoxelModel.h"
#include "../../GeomRendering/GeomData.h"
#include "../Constants.h"
#include "../../GeomRendering/DrawableGeom.h"

#ifdef ALWAYS_USE_GREEDY_MESH
#include "../../GreedyRendering/DrawableGreedy.h"
#endif

VoxelModel::VoxelModel() : meshes_(), worldPos_()
{
}

VoxelModel::VoxelModel(std::vector<PointColourIndex>& points, const glm::ivec3& maxSize, const glm::ivec3& minSize) : VoxelModel()
{
	auto index = [](unsigned int x, unsigned int y, unsigned int z) -> unsigned int { return x + y * CHUNK_SIZE_F + z * CHUNK_AREA; };
	typedef std::vector<Block> points_t;
	// contains an ordered array of all blocks in this chunk
	std::unordered_map<glm::ivec3, points_t> splitBlocks;
	for (PointColourIndex& point : points) {
		point.x += abs(minSize).x;
		point.y += abs(minSize).y;
		point.z += abs(minSize).z;
		// point should no longer have negative values
		glm::ivec3 p(point.x, point.y, point.z);
		p /= CHUNK_SIZE; // doesnt work if changed to * inverse

		point.x -= p.x * CHUNK_SIZE;
		point.y -= p.y * CHUNK_SIZE;
		point.z -= p.z * CHUNK_SIZE;

		if (p == glm::ivec3(2, 6, 7)) {
			int gbfijd = 0;
		}

		splitBlocks[p].resize(CHUNK_AREA * CHUNK_SIZE, B_AIR);
		splitBlocks[p][index(point.x, point.y, point.z)] = point.block;
	}
	for (auto& [p, cloud] : splitBlocks) {
		meshes_.emplace_back(p, cloud, splitBlocks);
		meshes_.back().parent = this;
	}
}

void VoxelModel::setPosition(float x, float y, float z)
{
	worldPos_ = { x, y, z };
}

void VoxelModel::addToDrawable(DrawableGeom& drawable)
{
	for (VoxelMesh& mesh : meshes_) {
		mesh.parent = this;
		drawable.add(&mesh);
	}
}
#ifdef ALWAYS_USE_GREEDY_MESH
void VoxelModel::addToDrawable(DrawableGreedy& drawable) const
{
	for (const VoxelMesh& mesh : meshes_) {
		drawable.add(const_cast<BufferGreedy*>(&mesh.greedyBuffer), (mesh.relativePos_ + worldPos_) * CHUNK_SIZE_INV);
	}
}
#endif
