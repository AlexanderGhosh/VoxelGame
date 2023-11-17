#include "DrawableRayMarching.h"
#include "BufferGeom.h"
#include "../Game/Player/Camera.h"
#include "../Game/World/Chunks/ChunkColumn.h"
#include "../Shaders/Shader.h"
#include "../Textures/Texture.h"

DrawableRayMarching::DrawableRayMarching() : data(), planeVAO(0)
{
}

DrawableRayMarching::~DrawableRayMarching()
{
	for (auto& t : data) {
		t.buffer->cleanUp();
	}
}

void DrawableRayMarching::render(Shader* shader) const
{
	if (!shader) {
		shader = &SHADERS[RAY_MARCHING];
		shader->bind();
	}

	shader->setValue("voxelSize", VOXEL_SZIE);

	draw(shader);

	shader->unBind();
}

void DrawableRayMarching::setUp(Chunks& chunks)
{
	data.clear();
	for (auto& [pos, chunk] : chunks) {
		data.emplace_back(chunk.getBufferPtr(), glm::vec3(pos.x, 0, pos.y));
	}
}

void DrawableRayMarching::add(ChunkColumn& chunk)
{
	const glm::vec2& pos = chunk.getPosition();
	data.emplace_back(chunk.getBufferPtr(), glm::vec3(pos.x, 0, pos.y));
}

void DrawableRayMarching::remove(const glm::vec2& chunkPos)
{
	for (auto itt = data.begin(); itt != data.end();) {
		DrawDataRayMarch& d = *itt;
		if (d.drawOrigin == glm::vec3(chunkPos.x, 0, chunkPos.y)) {
			itt = data.erase(itt);
		}
		else {
			itt++;
		}
	}
}

void DrawableRayMarching::setPlane(const unsigned int vao)
{
	planeVAO = vao;
}

void DrawableRayMarching::draw(Shader* shader) const
{
	for (const DrawDataRayMarch& data : this->data) {
		SSBOBuffer* buffer = data.buffer;
		shader->setValue("chunkPosition", data.drawOrigin * CHUNK_SIZE_F);

		buffer->bind();
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}
