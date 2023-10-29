#include "Drawable.h"
#include "Buffer.h"
Drawable::Drawable() {

}
Drawable::Drawable(Faces* sortedMeshes) {
	// setUp(sortedMeshes);
}
void Drawable::render(glm::mat4 lightProjection, glm::mat4 lightView)
{
	if (opaqueBuffer.size() == 0) return;
	Shader* shader = SHADERS[DEPTH];
	// shader->setUp();
	shader->bind();

	glm::mat4 LSM = lightProjection * lightView;

	shader->setValue("lightSpaceMatrix", LSM);

	draw(0);
	shader->unBind();
}
void Drawable::setUp(std::unordered_map<unsigned int, FaceB_p>& mesh) {
	clear();
	std::map<unsigned int, std::vector<glm::mat4>> positions;
	for (auto& pair : mesh) {
		const unsigned int key = pair.first;
		FaceB_p& faces = pair.second;
		Texture* texture = std::get<1>(pair.second);
		try {
			std::vector<glm::mat4>& poss = std::get<2>(faces);
			positions.at(key).insert(positions[key].end(), poss.begin(), poss.end());
			if (getDets(texture).isTransparant) {
				std::get<2>(transparantBuffer.at(key)) += poss.size();
			}
			else {
				std::get<2>(opaqueBuffer.at(key)) += poss.size();
			}
		}
		catch (std::exception e) {
			Buffer* buffer = std::get<0>(faces);
			std::vector<glm::mat4>& poss = std::get<2>(faces);

			positions.insert({ key, poss });
			if (getDets(texture).isTransparant) {
				transparantBuffer.insert({ key, { *buffer, texture, poss.size() } });
			}
			else {
				opaqueBuffer.insert({ key, { *buffer, texture, poss.size() } });
			}
		}
	}
	for (auto& b : opaqueBuffer) {
		const unsigned int key = b.first;
		FaceB& buff = b.second;
		Buffer& buf = std::get<0>(buff);
		buf.resetData();
		buf.addPositions(positions[key]);
	}
	for (auto& b : transparantBuffer) {
		const unsigned int key = b.first;
		FaceB& buff = b.second;
		Buffer& buf = std::get<0>(buff);
		buf.resetData();
		buf.addPositions(positions[key]);
	}
}
void Drawable::render(Camera& cam, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap) {
	if (opaqueBuffer.size() == 0) return;
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	Shader* shader = SHADERS[BLOCK3];
	// shader->setUp();
	shader->bind();

	glm::mat4 viewMatrix = cam.GetViewMatrix();
	shader->setValue("view", viewMatrix);
	shader->setValue("projection", projection);

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
;
void Drawable::draw(unsigned int depthMap, Shader* shader)
{
	for (auto& bu : opaqueBuffer) {
		FaceB& buffer = std::get<1>(bu);
		Buffer& b = std::get<0>(buffer);
		Texture* t = std::get<1>(buffer);
		unsigned int c = std::get<2>(buffer);
		t->bind();
		if (shader) {
			Material mat = getMaterial(t->getName());
			shader->setValue("material", mat);
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		b.render(0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, c);
		b.endRender();
		t->unBind();
	}
	for (auto& bu : transparantBuffer) {
		FaceB& buffer = std::get<1>(bu);
		Buffer& b = std::get<0>(buffer);
		Texture* t = std::get<1>(buffer);
		unsigned int c = std::get<2>(buffer);
		t->bind();
		if (shader) {
			Material mat = getMaterial(t->getName());
			shader->setValue("material", mat);
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		b.render(0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, c);
		b.endRender();
		t->unBind();
	}
}

void Drawable::clear() {
	for (auto& buffer : opaqueBuffer) {
		FaceB& faces = buffer.second;
		std::get<0>(faces).destroy();
	}
	opaqueBuffer.clear();
	for (auto& buffer : transparantBuffer) {
		FaceB& faces = buffer.second;
		std::get<0>(faces).destroy();
	}
	transparantBuffer.clear();
}