#include "Drawable.h"
Drawable::Drawable() {

}
Drawable::Drawable(std::vector<Face*>& sortedMeshes) {
	setUp(sortedMeshes);
	meshes = sortedMeshes;
}
void Drawable::setUp(std::vector<Face*> sortedMeshes, const char* overload) {
	if (sortedMeshes.size() == 0) return;
	buffers.clear();
	std::map<GLuint, std::vector<glm::mat4>> positions;
	for (auto& mesh : sortedMeshes) {
		Buffer*& b = std::get<0>(*mesh);
		Texture*& t = std::get<1>(*mesh);
		glm::vec3& p = std::get<2>(*mesh);
		GLuint key = std::pow(b->type + 1, 2) * t->getTexMap();
		glm::mat4 model(1);
		model = glm::translate(model, p);
		try {
			auto& face = buffers.at(key);
			std::get<2>(face)++;
			positions[key].push_back(model);
		}
		catch (std::exception e) {
			buffers.insert({ key, { *b, t, 1 } });
			positions.insert({ key, { model } });
		}
	}
	Buffer* front = FACES[FRONT];
	for (auto& b : buffers) {
		auto& key = std::get<0>(b);
		auto& buff = std::get<1>(b);
		auto& buf = std::get<0>(buff);
		buf.resetData();
		buf.addPositions(positions[key]);
	}
}
void Drawable::setUp(std::vector<Face*>& sortedMeshes) {
	if (sortedMeshes.size() == 0) return;
	buffers.clear();
	std::map<GLuint, std::vector<glm::mat4>> positions;
	for (auto& mesh : sortedMeshes) {
		Buffer*& b = std::get<0>(*mesh);
		Texture*& t = std::get<1>(*mesh);
		glm::vec3& p = std::get<2>(*mesh);
		GLuint key = std::pow(b->type+1, 2) * t->getTexMap();
		glm::mat4 model(1);
		model = glm::translate(model, p);
		try {
			auto& face = buffers.at(key);
			std::get<2>(face)++;
			positions[key].push_back(model);
		}
		catch (std::exception e) {
			buffers.insert({ key, { *b, t, 1 } });
			positions.insert({ key, { model } });
		}
	}
	Buffer* front = FACES[FRONT];
	for (auto& b : buffers) {
		auto& key = std::get<0>(b);
		auto& buff = std::get<1>(b);
		auto& buf = std::get<0>(buff);
		buf.resetData();
		buf.addPositions(positions[key]);
	}
}
void Drawable::render(Camera& cam, glm::mat4 projection) {
	if (buffers.size() == 0) return;
	Shader* shader = SHADERS[BLOCK3];
	shader->bind();

	glm::mat4 view(1);
	view = cam.GetViewMatrix();
	shader->setValue("view", view);
	shader->setValue("projection", projection);

	glm::vec3 viewPos = cam.GetPosition();
	shader->setValue("viewPos", viewPos);

	for (auto& bu : buffers) {
		FaceB& buffer = std::get<1>(bu);
		Buffer& b = std::get<0>(buffer);
		Texture* t = std::get<1>(buffer);
		GLuint c = std::get<2>(buffer);
		t->bind();
		b.render(0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, c);
		b.endRender();
		t->unBind();
	}
	shader->unBind();
}