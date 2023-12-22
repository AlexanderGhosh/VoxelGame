#include "VoxelModel_Base.h"
#include <vector_relational.hpp>
#include <algorithm>
#include "../../GeomRendering/GeomData.h"
#include "../Functions.h"
#include "../../Shaders/Shader.h"
#include "../Constants.h"
#include "../../GeomRendering/DrawableGeom.h"
#include "../../GreedyRendering/DrawableGreedy.h"

VoxelModel_Static::VoxelModel_Static() : meshes_(), worldPos_()
{
}

VoxelModel_Static::VoxelModel_Static(std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours, const glm::ivec3& maxSize, const glm::ivec3& minSize, bool hasCollider) : VoxelModel_Static()
{
	typedef std::vector<PointColourIndex> points_t;
	std::unordered_map<glm::ivec3, points_t> splitPoints;
	for (PointColourIndex& point : points) {
		point.x += abs(minSize).x;
		point.y += abs(minSize).y;
		point.z += abs(minSize).z;
		// point should no longer have negative values
		glm::ivec3 p(point.x, point.y, point.z);
		p /= CHUNK_SIZE;

		point.x -= p.x * CHUNK_SIZE;
		point.y -= p.y * CHUNK_SIZE;
		point.z -= p.z * CHUNK_SIZE;

		splitPoints[p].push_back(point);
	}
	for (auto& [p, points] : splitPoints) {
		glm::vec3 relativePos(p);
		relativePos *= CHUNK_SIZE_F;
		meshes_.emplace_back(relativePos, points, colours, hasCollider);
		meshes_.back().parent = this;
	}
}

void VoxelModel_Static::setPosition(float x, float y, float z)
{
	worldPos_ = { x, y, z };
}

void VoxelModel_Static::addToDrawable(DrawableGeom& drawable)
{
	for (VoxelMesh& mesh : meshes_) {
		mesh.parent = this;
		drawable.add(&mesh);
	}
}
#ifdef ALWAYS_USE_GREEDY_MESH
void VoxelModel_Static::addToDrawable(DrawableGreedy& drawable) const
{
	for (const VoxelMesh& mesh : meshes_) {
		drawable.add(const_cast<BufferGreedy*>(&mesh.greedyBuffer), (mesh.relativePos_ + worldPos_) * CHUNK_SIZE_INV);
	}
}
#endif
