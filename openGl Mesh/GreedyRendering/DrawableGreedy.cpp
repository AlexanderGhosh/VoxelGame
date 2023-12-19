#include "DrawableGreedy.h"
#include <glad/glad.h>

#include "GreedyDrawData.h"
#include "GreedyData.h"
#include "BufferGreedy.h"
#include "../Game/World/Chunks/ChunkColumn.h"
#include "../Shaders/Shader.h"

DrawableGreedy::DrawableGreedy() : data()
{
}

DrawableGreedy::~DrawableGreedy()
{
	for (auto& t : data) {
		t._buffer->cleanUp();
	}
}

void DrawableGreedy::render(Shader* shader) const
{
#ifdef RENDER_WIREFRAMES
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	shader->bind();
	shader->setValue("voxelSize", VOXEL_SIZE);

	draw(shader);

	shader->unBind();
#ifdef RENDER_WIREFRAMES
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

void DrawableGreedy::setUp(Chunks& chunks)
{
	data.clear();
	for (auto& [_, chunk] : chunks) {
		add(chunk);
	}
}

void DrawableGreedy::add(ChunkColumn& chunk)
{
#ifdef ALWAYS_USE_GREEDY_MESH
	data.emplace_back(chunk.getGreedyPtr(), chunk.getPosition3D());
#endif
}

void DrawableGreedy::remove(const glm::vec2& chunkPos)
{
	for (auto itt = data.begin(); itt != data.end();) {
		GreedyDrawData& d = *itt;
		if (d._drawOrigin == glm::vec3(chunkPos.x, 0, chunkPos.y)) {
			itt = data.erase(itt);
		}
		else {
			itt++;
		}
	}
}

void DrawableGreedy::draw(Shader* shader) const
{
	for (const GreedyDrawData& data : this->data) {
		BufferGreedy* buffer_ = data._buffer;

		shader->setValue("chunkPosition", data._drawOrigin * CHUNK_SIZE_F);

		buffer_->bind();
		glDrawArrays(GL_POINTS, 0, buffer_->size());
		buffer_->unbind();
	}
}
