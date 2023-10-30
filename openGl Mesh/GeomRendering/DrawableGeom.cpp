#include "DrawableGeom.h"

#include "BufferGeom.h"
// #include "GeomData.h"
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
		t.buffer.cleanUp();
	}
}

void DrawableGeom::render(Camera& cam, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap) const
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	const Shader& shader = SHADERS[GEOMBLOCKS];

	shader.bind();

	glm::mat4 viewMatrix = cam.GetViewMatrix();
	bool t = shader.setValue("view", viewMatrix);
	t = shader.setValue("projection", projection);

	glm::vec3 viewPos = cam.GetPosition();
	shader.setValue("viewPos", viewPos);
	shader.setValue("lightPos", LIGHTPOSITION);

	shader.setValue("lightSpaceMatrix", lightMatrix);

	shader.setValue("cubeMap", 0);
	shader.setValue("shadowMap", 1);


	draw(depthMap, shader);

	shader.unBind();
	glDisable(GL_CULL_FACE);
}

void DrawableGeom::setUp(const Chunks& chunks)
{
	data.clear();
	for (auto& chunk : chunks) {
		data.emplace_back(chunk.getBuffer(), nullptr);
	}
}

void DrawableGeom::draw(unsigned int depthMap, const Shader& shader) const
{
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (const DrawData& data : this->data) {
		const BufferGeom* buffer = &data.buffer;
		Texture* tex = data.texture;
		if (tex) {
			tex->bind();
		}

		buffer->bind();
		glDrawArrays(GL_POINTS, 0, buffer->size());
		buffer->unbind();
		if (tex) tex->unBind();
	}
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
