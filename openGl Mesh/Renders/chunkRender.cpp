#include "chunkRender.h"
namespace Render {
	ChunkMeshRender::ChunkMeshRender(std::string shaderName) {
		shader = new Shader(shaderName);
		canRender = GL_FALSE;
	}
	ChunkMeshRender::ChunkMeshRender(Shader* shader) {
		this->shader = shader;
	}
	ChunkMeshRender::ChunkMeshRender(GLboolean init, std::string t) {
		if (init) {
			shader = SHADERS[BLOCK2];
			canRender = GL_FALSE;
		}
		else {
			canRender = GL_FALSE;
		}
	}
	void ChunkMeshRender::render(Camera p1, glm::mat4 projection) {
			if (!canRender) {
				std::cout << "Unable to render please call 'loadMesh()'" << std::endl;
				return;
			}
			shader->bind();

			glm::mat4 view(1);
			view = p1.GetViewMatrix();
			shader->setValue("view", view);
			shader->setValue("projection", projection);

			glm::vec3 viewPos = p1.GetPosition();
			shader->setValue("viewPos", viewPos);

			auto createModel = [](glm::vec3 position, glm::vec3 rotation, Camera& p1, Shader* shader) {
				glm::mat4 model(1);
				model = glm::translate(model, position);
				if (rotation.x != 0) {
					model = glm::rotate(model, rotation.x, { 0, 1, 0 });
				}
				shader->setValue("model", model);
			};

			for (auto& mesh: *meshes) {
				createModel(std::get<2>(mesh), { 0, 0, 0 }, p1, shader);;
				std::get<1>(mesh)->bind();
				std::get<0>(mesh)->render();
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			shader->unBind();
	}
	void ChunkMeshRender::loadMeshes(std::vector<Face>* m) {
		meshes = m;
		canRender = GL_TRUE;
	}
	void ChunkMeshRender::destroy() {
		//buffer.destroy();
	}
	void ChunkMeshRender::setPosition(glm::vec3 position) {
		for (auto& face : *meshes) {
			std::get<2>(face) = position;
		}
	}
	void ChunkMeshRender::setShader(std::string name) {
		shader = new Shader(name);
	}
	void ChunkMeshRender::setShader(Shader* shader) {
		this->shader = shader;
	}
	void ChunkMeshRender::addPosition(glm::vec3 positon) {
		for (auto& face : *meshes) {
			std::get<2>(face) += positon;
		}
	}
	void ChunkMeshRender::cleanUp() {
		shader->unBind();
		canRender = false;
		meshes = new std::vector<Face>();
	}
};