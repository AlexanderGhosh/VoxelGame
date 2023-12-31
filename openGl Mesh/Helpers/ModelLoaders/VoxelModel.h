#pragma once
#include <type_traits>
#include <vector>
#include <list>
#include <./ext/vector_int3.hpp>
#include "VoxelMesh.h"

class PointColourIndex;
class DrawableGeom;
class DrawableGreedy;

class VoxelModel {
	friend class VoxelMesh;
public:
	VoxelModel();
	inline bool isEditable() const { return false; }

	VoxelModel(std::vector<PointColourIndex>& points, const glm::ivec3& maxSize, const glm::ivec3& minSize);

	void setPosition(float x, float y, float z);

	void addToDrawable(DrawableGeom& drawable); 
#ifdef ALWAYS_USE_GREEDY_MESH
	void addToDrawable(DrawableGreedy& drawable) const;
#endif
protected:
	std::list<VoxelMesh> meshes_;
	glm::vec3 worldPos_;
};