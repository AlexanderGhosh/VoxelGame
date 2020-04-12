#include "Drawable.h"
Drawable::Drawable() {

}
Drawable::Drawable(Faces* sortedMeshes) {
	// setUp(sortedMeshes);
}
void Drawable::render(glm::mat4 lightProjection, glm::mat4 lightView)
{
	if (buffers.size() == 0) return;
	Shader* shader = SHADERS[DEPTH];
	shader->setUp();
	shader->bind();

	glm::mat4 LSM = lightProjection * lightView;

	shader->setValue("lightSpaceMatrix", LSM);

	draw(0);
	shader->unBind();
}
void Drawable::setUp(std::unordered_map<GLuint, FaceB_p>& mesh) {
	
	clear();
	std::map<GLuint, std::vector<glm::mat4>> positions;
	for (auto& pair : mesh) {
		const GLuint key = pair.first;
		FaceB_p& faces = pair.second;
		try {
			std::vector<glm::mat4>& poss = std::get<2>(faces);
			positions.at(key).insert(positions[key].end(), poss.begin(), poss.end());
			std::get<2>(buffers.at(key)) += poss.size();
		}
		catch (std::exception e) {
			Buffer* buffer = std::get<0>(faces);
			Texture* texture = std::get<1>(faces);
			std::vector<glm::mat4>& poss = std::get<2>(faces);

			positions.insert({ key, poss });
			buffers.insert({ key, { *buffer, texture, poss.size() } });
		}
	}
	for (auto& b : buffers) {
		const GLuint key = b.first;
		FaceB& buff = b.second;
		Buffer& buf = std::get<0>(buff);
		buf.resetData();
		buf.addPositions(positions[key]);
	}
}
void Drawable::render(Camera& cam, glm::mat4 projection, glm::mat4 lightMatrix, GLuint depthMap) {
	if (buffers.size() == 0) return;
	// glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	Shader* shader = SHADERS[BLOCK3];
	shader->setUp();
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

	draw(depthMap);
	shader->unBind();
	glDisable(GL_CULL_FACE);
	// glDisable(GL_FRAMEBUFFER_SRGB);
}

Texture t;
GLboolean first = 1;
void Drawable::draw(GLuint depthMap)
{
	if (first) {
		t = Texture("grass/hd/top", 1);
		first = 0;
	}
	// t.load2D(t.getName());
	for (auto& bu : buffers) {
		FaceB& buffer = std::get<1>(bu);
		Buffer& b = std::get<0>(buffer);
		Texture* t = std::get<1>(buffer);
		GLuint c = std::get<2>(buffer);
		t->bind();
		// t.bind();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		b.render(0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, c);
		b.endRender();
		t->unBind();
		// t.unBind();
	}
}

void Drawable::clear() {
	for (auto& buffer : buffers) {
		FaceB& faces = buffer.second;
		std::get<0>(faces).destroy();
	}
	buffers.clear();
}