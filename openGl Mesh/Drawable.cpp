#include "Drawable.h"
Drawable::Drawable() {

}
Drawable::Drawable(Faces* sortedMeshes) {
	// setUp(sortedMeshes);
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

void Drawable::clear() {
	for (auto& buffer : buffers) {
		FaceB& faces = buffer.second;
		std::get<0>(faces).destroy();
	}
	buffers.clear();
}