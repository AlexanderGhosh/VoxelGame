#pragma once
#include <type_traits>
#include <vector>
#include <list>
#include <./ext/vector_int3.hpp>
#include "../../GeomRendering/BufferGeom.h"
#include "VoxelMesh.h"

class Shader;
class PointColourIndex;


class VoxelModel_Static {
public:
	VoxelModel_Static();
	inline bool isEditable() const { return false; }

	VoxelModel_Static(std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours, const glm::ivec3& maxSize, const glm::ivec3& minSize);

	void render(const Shader& shader) const;
protected:
	std::list<VoxelMesh> meshes_;
	glm::vec3 worldPos_;
};

//class VoxelModel_Dynamic : public VoxelModel_Static {
//public:
//	VoxelModel_Dynamic();
//
//	VoxelModel_Dynamic(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& colours);
//
//	inline bool isEditable() const { return true; }
//private:
//	std::vector<GeomData> meshData;
//};