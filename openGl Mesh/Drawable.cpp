#include "Drawable.h"
Drawable::Drawable() {

}
Drawable::Drawable(Faces* sortedMeshes) {
	// setUp(sortedMeshes);
}
void Drawable::setUp(std::unordered_map<GLuint, FaceB_p>& mesh) {
	/*Faces& meshes = *mesh;
	if (meshes.size() == 0) return;
	buffers.clear();
	std::map<GLuint, std::vector<glm::mat4>> positions;
	for (auto& mesh : meshes) {
		Buffer* b = std::get<0>(mesh);
		Texture* t = std::get<1>(mesh);
		glm::vec3 p = std::get<2>(mesh);
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
	}*/
	buffers.clear();
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