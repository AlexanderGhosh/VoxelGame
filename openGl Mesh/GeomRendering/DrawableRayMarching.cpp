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
		t.noiseData.cleanUp();
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
		data.emplace_back(chunk.getBuffer(), glm::vec3(pos.x, 0, pos.y), chunk.getMaxHeight());
	}
}

void DrawableRayMarching::add(ChunkColumn& chunk)
{
	const glm::vec2& pos = chunk.getPosition();
	data.emplace_back(chunk.getBuffer(), glm::vec3(pos.x, 0, pos.y), chunk.getMaxHeight());
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
		const Texture& noise = data.noiseData;
		shader->setValue("chunkPosition", data.drawOrigin * CHUNK_SIZE_F);
		shader->setValue("maxHeight", data.maxHeight);

		glActiveTexture(GL_TEXTURE0);
		noise.bind();
		bool a = shader->setValue("geometry", 0);

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}