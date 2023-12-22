#pragma once
#include <type_traits>
#include <vector>
#include <list>
#include <./ext/vector_int3.hpp>
#include "../../GeomRendering/BufferGeom.h"
#include "VoxelMesh.h"

class Shader;
class PointColourIndex;
class DrawableGeom;
class DrawableGreedy;

class VoxelModel_Static {
	friend class VoxelMesh;
public:
	VoxelModel_Static();
	inline bool isEditable() const { return false; }

	VoxelModel_Static(std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours, const glm::ivec3& maxSize, const glm::ivec3& minSize, bool hasCollider);

	void setPosition(float x, float y, float z);

	void addToDrawable(DrawableGeom& drawable); 
#ifdef ALWAYS_USE_GREEDY_MESH
	void addToDrawable(DrawableGreedy& drawable) const;
#endif
protected:
	std::list<VoxelMesh> meshes_;
	glm::vec3 worldPos_;
};