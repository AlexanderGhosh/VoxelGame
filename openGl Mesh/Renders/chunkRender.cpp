#include "chunkRender.h"
namespace Render {
	ChunkMeshRender::ChunkMeshRender(std::string shaderName) {
		shader = new Shader(shaderName);
		canRender = GL_FALSE;
	}
	ChunkMeshRender::ChunkMeshRender(Shader* shader) {
		this->shader = shader;
	}
	ChunkMeshRender::ChunkMeshRender(GLboolean init) {
		if (init) {
			shader = SHADERS[BLOCK2];
			canRender = GL_FALSE;
		}
		else {
			canRender = GL_FALSE;
		}
	}
	int quadraticFormula(int meshCount) {
		//6x^2 - 12x + 8 = meshCount
		int c = 8 - meshCount;
		float t = std::powf((144 - 24 * c), 0.5f);
		t /= 12;
		if (1 + t > 0) return 1 + t;
		return 1 - t;
	}
	void ChunkMeshRender::render(Camera p1, glm::mat4 projection) {
		if (!canRender) {
			std::cout << "Unable to render please call 'loadMesh()'" << std::endl;
			return;
		}
		shader->bind();

		glm::mat4 view(1);
		view = p1.GetViewMatrix();
		if (!shader->setValue("view", view)) {
			std::cout << "shader not working" << std::endl;
			shader = SHADERS[BLOCK2];
			for (auto& mesh : *meshes) {
				// mesh.setTexture("grass");
				mesh.texture = TEXTURES[GRASS];
			}
		}
		shader->setValue("projection", projection);

		/*glm::vec3 objCol(1, 0.5, 0.31), lightCol(1);
		shader->setValue("objCol", objCol);
		shader->setValue("lightCol", lightCol);*/

		glm::vec3 viewPos = p1.GetPosition();
		shader->setValue("viewPos", viewPos);

		Texture* prevTex = nullptr;
		Buffer* prevBuffer = nullptr;
		std::vector<Buffer*> prevCombo = std::vector<Buffer*>();

		auto createModel = [](glm::vec3 position, glm::vec3 rotation, Camera& p1, Shader* shader) {
			glm::mat4 model(1);
			model = glm::translate(model, position);
			if (rotation.x != 0) {
				model = glm::rotate(model, rotation.x, { 0, p1.GetPosition().y, 0 });
			}
			shader->setValue("model", model);
		};

		for (auto& mesh : *meshes) {
			
			createModel(mesh.position, mesh.rotation, p1, shader);

			if (mesh.texture != prevTex) {
				mesh.texture->bind();
				prevTex = mesh.texture;
			}

			// 26
			if (mesh.getBuffer() != prevBuffer) {
				if (mesh.isCombo() && mesh.comboOf == prevCombo) {
					mesh.getBuffer()->draw();
				}
				else {
					mesh.getBuffer()->render();
					prevBuffer = mesh.getBuffer();
					prevCombo = mesh.comboOf;
				}
			}
			else {
				mesh.getBuffer()->draw();
			}
		}
		glBindVertexArray(0);
		shader->unBind();
	}
	void ChunkMeshRender::loadMeshes(std::vector<Mesh::FaceMesh>* m) {
		meshes = m;
		canRender = GL_TRUE;
	}
	void ChunkMeshRender::destroy() {
		//buffer.destroy();
	}
	void ChunkMeshRender::setPosition(glm::vec3 position) {
		for (auto& face : *meshes) {
			face.setPosition(position);
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
			face.setPosition(face.position + positon);
		}
	}
	void ChunkMeshRender::setRotaion(const glm::vec3& rotation) {
		for (auto& face : *meshes) {
			face.setRotation(rotation);
		}
	}
	void ChunkMeshRender::cleanUp() {
		shader->unBind();
		canRender = false;
		meshes = new std::vector<Mesh::FaceMesh>();
	}
};