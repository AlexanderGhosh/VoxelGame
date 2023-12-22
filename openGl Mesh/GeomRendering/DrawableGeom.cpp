#include "DrawableGeom.h"
#include <glad/glad.h>
#include "../Helpers/Constants.h"
#include "DrawData.h"
#include "BufferGeom.h"
#include "../Game/World/Chunks/ChunkColumn.h"
#include "../Shaders/Shader.h"
#include "IGeomDrawable.h"

DrawableGeom::DrawableGeom() : data()
{
}

DrawableGeom::~DrawableGeom()
{
}

void DrawableGeom::render(Shader* shader) const
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

void DrawableGeom::setUp(Chunks& chunks)
{
	data.clear();
	for (auto& [pos, chunk] : chunks) {
		add(&chunk);
	}
}

void DrawableGeom::remove(const glm::vec2& chunkPos)
{
	for (auto itt = data.begin(); itt != data.end();) {
		DrawData& d = *itt;
		if (d.type_ == DrawData::CHUNK && d.drawOrigin_ == glm::vec3(chunkPos.x, 0, chunkPos.y)) {
			itt = data.erase(itt);
		}
		else {
			itt++;
		}
	}
}

DrawData* DrawableGeom::get(BufferGeom* buffer)
{
	for (DrawData& data : this->data) {
		const BufferGeom* buffer_ = data.buffer_;
		if (buffer_ == buffer) return &data;
	}
	return nullptr;
}

void DrawableGeom::add(const IGeomDrawable* elem)
{
	data.push_back(elem->getDrawData());
}

void DrawableGeom::remove(IGeomDrawable* elem)
{
	for (auto itt = data.begin(); itt != data.end();) {
		DrawData& d = *itt;
		if (d == elem->getDrawData()) {
			itt = data.erase(itt);
			return;
		}
		else {
			itt++;
		}
	}
}

void DrawableGeom::draw(Shader* shader) const
{
	for (const DrawData& data : this->data) {
		const BufferGeom* buffer_ = data.buffer_;
		shader->setValue("chunkPosition", data.drawOrigin_);
		
		buffer_->bind();
		glDrawArrays(GL_POINTS, 0, buffer_->size());
		buffer_->unbind();
	}
}
