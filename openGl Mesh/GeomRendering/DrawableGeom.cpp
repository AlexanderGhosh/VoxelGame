#include "DrawableGeom.h"
#include "DrawData.h"
#include "BufferGeom.h"
#include "../Game/World/Chunks/ChunkColumn.h"
#include "../Shaders/Shader.h"
#include "../Textures/Texture.h"
#include <glad/glad.h>

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
	shader->setValue("voxelSize", VOXEL_SIZE);

	draw(shader);

	shader->unBind();
}

void DrawableGeom::setUp(Chunks& chunks)
{
	data.clear();
	for (auto& [pos, chunk] : chunks) {
		data.emplace_back(chunk.getBufferPtr(), nullptr, chunk.getPosition3D());
	}
}

void DrawableGeom::add(ChunkColumn& chunk)
{
	data.emplace_back(chunk.getBufferPtr(), nullptr, chunk.getPosition3D());
}

void DrawableGeom::remove(const glm::vec2& chunkPos)
{
	for (auto itt = data.begin(); itt != data.end();) {
		DrawData& d = *itt;
		if (d.drawOrigin == glm::vec3(chunkPos.x, 0, chunkPos.y)) {
			itt = data.erase(itt);
		}
		else {
			itt++;
		}
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
		auto e = glGetError();
		if (e != 0) {
			buffer->unbind();
		}
		buffer->unbind();
		if (tex) tex->unBind();
	}
}
