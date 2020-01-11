#include "chunkRender.h"
namespace Render {
	ChunkMeshRender::ChunkMeshRender(std::string shaderName) : shader("block2") {
		shader = Shader(shaderName);
		canRender = GL_FALSE;
	}
	void ChunkMeshRender::render(Camera p1, glm::mat4 projection) {
		if (!canRender) {
			std::cout << "Unable to render please call 'loadMesh()'" << std::endl;
			return;
		}
		shader.bind();

		glm::mat4 view(1);
		view = p1.GetViewMatrix();

		if (!shader.setValue("view", view)) {
			std::cout << "shader not working" << std::endl;
			shader = Shader("block2");
			for (auto& mesh : meshes) {
				mesh.setTexture("grass");
			}
		}
		shader.setValue("projection", projection);

		glm::vec3 objCol(1, 0.5, 0.31), lightCol(1);
		shader.setValue("objCol", objCol);
		shader.setValue("lightCol", lightCol);

		glm::vec3 viewPos = p1.GetPostion();
		shader.setValue("viewPos", viewPos);

		
		for (auto& mesh : meshes) {
			glm::mat4 model(1);
			model = glm::translate(model, mesh.position);
			shader.setValue("model", model);
			mesh.bindTexture();
			mesh.buffer.render();
		}
	}

	void ChunkMeshRender::loadMeshes(std::vector<Mesh::FaceMesh>& m) {
		meshes = m;
		canRender = GL_TRUE;
	}
	void ChunkMeshRender::destroy() {

		//buffer.destroy();
	}
};