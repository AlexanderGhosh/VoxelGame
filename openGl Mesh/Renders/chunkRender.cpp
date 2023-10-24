#include "chunkRender.h"
#include "../Buffer.h"
namespace Render {
	ChunkMeshRender::ChunkMeshRender() {
		canRender = GL_FALSE;
	}
	ChunkMeshRender::ChunkMeshRender(std::string shaderName) {
		shader = Shader(shaderName);
		canRender = GL_FALSE;
	}
	ChunkMeshRender::ChunkMeshRender(Shader* shader) {
		this->shader = *shader;
	}
	ChunkMeshRender::ChunkMeshRender(GLboolean init, std::string t) {
		if (init) {
			shader = Shader("block2");
			shader.setUp();
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
			shader.bind();

			glm::mat4 view(1);
			view = p1.GetViewMatrix();
			shader.setValue("view", view);
			shader.setValue("projection", projection);

			glm::vec3 viewPos = p1.GetPosition();
			shader.setValue("viewPos", viewPos);

			

			auto createModel = [](glm::vec3 position, GLfloat angle, glm::vec3 axis, Camera& p1, Shader shader) {
				glm::mat4 model(1);
				model = glm::translate(model, position);
				model = glm::rotate(model, glm::radians(angle), axis);

				model = glm::scale(model, glm::vec3(0.8f));
				shader.setValue("model", model);
			};

			for (auto& mesh : *meshes) {
				createModel(std::get<2>(mesh), angle, axis, p1, shader);
				std::get<1>(mesh)->bind();
				std::get<0>(mesh)->render();
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			shader.unBind();
	}
	void ChunkMeshRender::loadMeshes(std::vector<Face>* m) {
		meshes = m;
		canRender = GL_TRUE;
	}
	void ChunkMeshRender::destroy() {
		//buffer.destroy();
	}
	void ChunkMeshRender::setPosition(glm::vec3 position) {
		/*for (auto& face : *meshes) {
			std::get<2>(face) = position;
		}*/
		for (GLubyte i = 0; i < (*meshes).size(); i++) {
			glm::vec3 relativePos(0);
			if (relativePositions.size() >= i) {
				relativePos = relativePositions[i];
			}
			std::get<2>((*meshes)[i]) = position + relativePos;
		}
	}
	void ChunkMeshRender::setShader(std::string name) {
		shader = Shader(name);
	}
	void ChunkMeshRender::setShader(Shader* shader) {
		this->shader = *shader;
	}
	void ChunkMeshRender::setRelativePositions(std::vector<glm::vec3>& rel)
	{
		relativePositions = rel;
	}
	void ChunkMeshRender::setRotation(glm::vec3 rotAxis, GLfloat rotAngle) {
		angle = rotAngle;
		axis = rotAxis;
	}
	void ChunkMeshRender::addPosition(glm::vec3 positon) {
		for (auto& face : *meshes) {
			std::get<2>(face) += positon;
		}
	}
	void ChunkMeshRender::cleanUp() {
		shader.unBind();
		canRender = false;
		meshes = new std::vector<Face>();
	}
	Shader& ChunkMeshRender::getShader() {
		return shader;
	}
};