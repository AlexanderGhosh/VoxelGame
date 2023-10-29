#include "DrawableGeom.h"
#include "../Game/Player/Camera.h"
#include "../Shaders/Shader.h"
#include "../Game/World/constants.h"
#include "BufferGeom.h"
#include "GeomData.h"
#include "../Buffer.h"
#include "../Game/World/Chunks/ChunkColumn.h"

DrawableGeom::DrawableGeom() : data()
{
}

DrawableGeom::~DrawableGeom()
{
	for (auto& t : data) {
		t.buffer.cleanUp();
	}
}

void DrawableGeom::render(Camera& cam, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap)
{
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	Shader* shader = SHADERS[GEOMBLOCKS];

	shader->bind();

	glm::mat4 viewMatrix = cam.GetViewMatrix();
	bool t = shader->setValue("view", viewMatrix);
	t = shader->setValue("projection", projection);

	glm::vec3 viewPos = cam.GetPosition();
	shader->setValue("viewPos", viewPos);
	shader->setValue("lightPos", LIGHTPOSITION);

	shader->setValue("lightSpaceMatrix", lightMatrix);

	shader->setValue("cubeMap", 0);
	shader->setValue("shadowMap", 1);


	draw(depthMap, shader);

	shader->unBind();
	glDisable(GL_CULL_FACE);
}

void DrawableGeom::setUp(const std::unordered_map<GLuint, FaceB_p>& mesh)
{
	for (auto& d : data) {
		d.buffer.cleanUp();
	}
	data.clear();
	for (auto& m : mesh) {
		const FaceB_p& d = m.second;
		const auto& translations = std::get<2>(d);
		BufferGeom buffer;
		std::vector<GeomData> raw;
		raw.reserve(translations.size());
		
		for (const glm::mat4& translation: translations) {
			const glm::vec3 pos = translation * glm::vec4(1);
			GeomData geomData;
			geomData.worldPos_ = pos;
			geomData.cubeType_ = std::get<0>(d)->type;
			raw.push_back(geomData);
		}

		buffer.setUp(raw.data(), raw.size());

		data.emplace_back(buffer, std::get<1>(d));
	}
}

void DrawableGeom::setUp(const Chunks& chunks)
{
	data.clear();
	for (auto& chunk : chunks) {
		data.emplace_back(chunk.buffer, nullptr);
	}
}

void DrawableGeom::draw(unsigned int depthMap, Shader* shader)
{
	for (const DrawData& data : this->data) {
		const BufferGeom* buffer = &data.buffer;
		Texture* tex = data.texture;
		if (tex) {
			tex->bind();

			Material mat = getMaterial(tex->getName());
			shader->setValue("material", mat);
		}

		buffer->bind();
		glDrawArrays(GL_POINTS, 0, buffer->size());
		buffer->unbind();
		if (tex) tex->unBind();
	}
}
