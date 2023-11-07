#include "DrawableGeom.h"
#include "DrawData.h"
#include "BufferGeom.h"
#include "../Game/Player/Camera.h"
#include "../Game/World/Chunks/ChunkColumn.h"
#include "../Shaders/Shader.h"
#include "../Textures/Texture.h"

DrawableGeom::DrawableGeom() : data()
{
}

DrawableGeom::~DrawableGeom()
{
	for (auto& t : data) {
		t.buffer->cleanUp();
	}
}

void DrawableGeom::render(Shader* shader) const
{
	if (!shader) {
		shader = &SHADERS[GEOMBLOCKS];
		shader->bind();
	}

	shader->setValue("voxelSize", VOXEL_SZIE);

	draw(shader);

	shader->unBind();
}

void DrawableGeom::setUp(Chunks& chunks)
{
	data.clear();
	for (auto& [pos, chunk] : chunks) {
		data.emplace_back(chunk.getBufferPtr(), nullptr, glm::vec3(pos.x, 0, pos.y));
	}
}

void DrawableGeom::draw(Shader* shader) const
{
	for (const DrawData& data : this->data) {
		BufferGeom* buffer = data.buffer;
		Texture* tex = data.texture;
		if (tex) {
			tex->bind();
		}
		shader->setValue("chunkPosition", data.drawOrigin * CHUNK_SIZE_F);


		buffer->bind();
		glDrawArrays(GL_POINTS, 0, buffer->size());
		buffer->unbind();
		if (tex) tex->unBind();
	}
}
