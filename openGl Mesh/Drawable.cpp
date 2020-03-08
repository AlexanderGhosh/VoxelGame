#include "Drawable.h"
Drawable::Drawable() {

}
Drawable::Drawable(std::vector<Face*>& sortedMeshes) {
	setUp(sortedMeshes);
	meshes = sortedMeshes;
}
void Drawable::setUp(std::vector<Face*> sortedMeshes, const char* overload) {
	if (sortedMeshes.size() < 1) return;
	sortedMeshes = sortMesh(sortedMeshes);
	meshes = sortedMeshes;
	buffers.clear();

	Buffer* prevBuffer = std::get<0>(*sortedMeshes[0]);
	Texture* prevTex = std::get<1>(*sortedMeshes[0]);
	GLuint counter = 0;
	std::vector<glm::mat4> positions;

	for (auto& mesh : sortedMeshes) {
		if (std::get<0>(*mesh) != prevBuffer || std::get<1>(*mesh) != prevTex) {
			prevBuffer->addPositions(positions);
			buffers.push_back({ *prevBuffer, prevTex, counter });
			prevBuffer->resetData();
			// reset
			counter = 0;
			positions.clear();
			prevBuffer = std::get<0>(*mesh);
			prevTex = std::get<1>(*mesh);
			prevBuffer->resetData();
		}
		glm::mat4 model(1);
		model = glm::translate(model, std::get<2>(*mesh));
		positions.push_back(model);
		counter += 1;
	}
	prevBuffer->addPositions(positions);
	buffers.push_back({ *prevBuffer, prevTex, counter });
}
void Drawable::setUp(std::vector<Face*>& sortedMeshes) {
	if (sortedMeshes.size() < 1) return;
	sortedMeshes = sortMesh(sortedMeshes);
	meshes = sortedMeshes;
	buffers.clear();

	Buffer* prevBuffer = std::get<0>(*sortedMeshes[0]);
	Texture* prevTex = std::get<1>(*sortedMeshes[0]);
	GLuint counter = 0;
	std::vector<glm::mat4> positions;

	for (auto& mesh : sortedMeshes) {
		if (std::get<0>(*mesh) != prevBuffer || std::get<1>(*mesh) != prevTex) {
			prevBuffer->addPositions(positions);
			buffers.push_back({ *prevBuffer, prevTex, counter });
			prevBuffer->resetData();
			// reset
			counter = 0;
			positions.clear();
			prevBuffer = std::get<0>(*mesh);
			prevTex = std::get<1>(*mesh);
			prevBuffer->resetData();
		}
		glm::mat4 model(1);
		model = glm::translate(model, std::get<2>(*mesh));
		positions.push_back(model);
		counter += 1;
	}
	prevBuffer->addPositions(positions);
	buffers.push_back({ *prevBuffer, prevTex, counter });
}
void Drawable::render(Camera& cam, glm::mat4 projection) {
	if (buffers.size() < 1) return;
	Shader* shader = SHADERS[BLOCK3];
	shader->bind();

	glm::mat4 view(1);
	view = cam.GetViewMatrix();
	shader->setValue("view", view);
	shader->setValue("projection", projection);

	glm::vec3 viewPos = cam.GetPosition();
	shader->setValue("viewPos", viewPos);

	for (auto& buffer : buffers) {
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
std::vector<Face*> Drawable::sortMesh(std::vector<Face*>& meshes) {

	std::vector<Face*> res = meshes;
	std::sort(res.begin(), res.end());
	return res;
	/*std::map<Texture*, std::vector<GLuint>> texture_inices;
	for (int i = 0; i < meshes.size(); i++) {
		auto& mesh = meshes[i];
		try {
			texture_inices[std::get<1>(*mesh)].push_back(i);
		}
		catch (std::exception e) {
			texture_inices.insert({ std::get<1>(*mesh), {} });
		}
	}
	std::map<int, std::vector<GLuint>> buffer_tex_split;
	for (auto& pair : texture_inices) {
		for (auto& index : pair.second) {
			buffer_tex_split[(int)pair.first * (int)std::get<1>(*meshes[index])].push_back(index);
		}
	}
	std::vector<Face*> res;
	for (auto& pair : buffer_tex_split) {
		for (auto& index : pair.second) {
			res.push_back(meshes[index]);
		}
	}
	return res;*/
}