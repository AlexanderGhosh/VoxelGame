#include "meshRender.h"
namespace Render {
	FaceMeshRender::FaceMeshRender(std::string shaderName) : shader(shaderName) {
		canRender = GL_FALSE;
	}
	void FaceMeshRender::render(Camera p1, glm::mat4 projection) {
		if (!canRender) {
			std::cout << "Unable to render please call 'loadMesh()'" << std::endl;
			return;
		}
		shader.bind();

		glm::mat4 view(1);
		view = p1.GetViewMatrix();

		shader.setValue("view", view);
		shader.setValue("projection", projection);

		glm::vec3 objCol(1, 0.5, 0.31), lightCol(1);
		shader.setValue("objCol", objCol);
		shader.setValue("lightCol", lightCol);

		glm::mat4 model(1);
		model = glm::translate(model, mesh.position);

		shader.setValue("model", model);
		glm::vec3 viewPos = p1.GetPostion();
		shader.setValue("viewPos", viewPos);

		mesh.bindTexture();
		mesh.buffer.render();
	}

	void FaceMeshRender::loadMesh(Mesh::FaceMesh& m) {
		mesh = m;
		buffer.merge(m.buffer);
		canRender = GL_TRUE;
	}
	void FaceMeshRender::destroy() {
		buffer.destroy();
	}
	void FaceMeshRender::setTexture(std::string name) {
		mesh.setTexture(name);
	}
	void FaceMeshRender::setPosition(glm::vec3 position) {
		mesh.setPosition(position);
	}
};