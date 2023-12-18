#include "VoxelMesh.h"
#include "../Constants.h"
#include "../Functions.h"
#include "../../GeomRendering/GeomData.h"
#include "../../Shaders/Shader.h"
#include <glad/glad.h>

VoxelMesh::VoxelMesh() : relativePos_(), buffer_()
{
}

VoxelMesh::VoxelMesh(const glm::vec3& relativePos, std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours) : VoxelMesh()
{
	relativePos_ = relativePos;
	const glm::vec3 span(CHUNK_SIZE_F);
	auto index = [span](unsigned int x, unsigned int y, unsigned int z) -> unsigned int { return x + y * span.x + z * span.x * span.y; };
	int size = span.x * span.y * span.z;
	std::vector<Block> cloud(size, Block::AIR); // -1 if empty
	for (unsigned int i = 0; i < points.size(); i++) {
		PointColourIndex& point = points[i];

		const int idx = index(point.x, point.y, point.z);
		cloud[idx] = Block::STONE;
	}

	std::vector<GeomData> bufferData;
	bufferData.reserve(size);

	for (unsigned int x = 0; x < span.x; x++) {
		for (unsigned int z = 0; z < span.z; z++) {
			for (unsigned int y = 0; y < span.y; y++) {
				const Block current = cloud[index(x, y, z)];
				const BlockDetails& currentDetails = getDetails(current);

				if (current == Block::AIR) continue;

				GeomData data;
				data.setPos({ x, y, z });
				data.textureIndex_ = (unsigned char)current;
				for (unsigned int i = 0; i < OFFSETS_3D.size(); i++) {
					glm::vec3 currentPos(x, y, z);
					glm::vec3 neighbourPos = currentPos + OFFSETS_3D[i];
					Block neighbour = Block::AIR;

					if (glm::all(glm::greaterThanEqual(neighbourPos, glm::vec3(0))) && glm::all(glm::lessThan(neighbourPos, span))) {
						neighbour = cloud[index(neighbourPos.x, neighbourPos.y, neighbourPos.z)];
					}

					const BlockDetails& neighboursDetails = getDetails(neighbour);

					if (neighboursDetails.isTransparant && current != neighbour) {
						markSlot(data.cubeType_, i);
					}
				}
				bufferData.push_back(data);
			}
		}
	}
	bufferData.shrink_to_fit();
	buffer_.setUp(bufferData.data(), bufferData.size());
}

void VoxelMesh::render(const Shader& shader, const glm::vec3& parentPos) const
{
	shader.setValue("chunkPosition", relativePos_ + parentPos);

	buffer_.bind();
	glDrawArrays(GL_POINTS, 0, buffer_.size());
}
