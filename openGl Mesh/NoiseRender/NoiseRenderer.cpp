#include "NoiseRenderer.h"
#include <glad/glad.h>
#include "NoiseBuffer.h"
#include "../Helpers/Constants.h"
#include "../Shaders/Shader.h"

void NoiseRenderer::add(const NoiseRenderData& data)
{
	data_.push_back(data);
}

void NoiseRenderer::render()
{
	Shader& shader = SHADERS[RENDER_NOISE];
	shader.bind();

	shader.setValue("voxelSize", VOXEL_SIZE);
	for (const NoiseRenderData& data : data_) {
		data.buffer_->bind();
		shader.setValue("chunkPosition", data.chunkPos_);
		glDrawArrays(GL_POINTS, 0, data.buffer_->size());
	}
}
