#include "ModelRenderer.h"
#include <glad/glad.h>
#include "../IndexedBuffer.h"
#include "../Helpers/ModelLoaders/Model.h"

ModelRenderer::ModelRenderer() : models()
{
}

ModelRenderer::~ModelRenderer()
{
	cleanUp();
}

void ModelRenderer::add(Model& buffer)
{
	models.push_back(buffer);
}

void ModelRenderer::render(Shader* shader)
{
	for (auto& model : models) {
		for (auto& buffer : model.meshBuffers) {
			buffer.bind();
			glDrawElements(GL_TRIANGLES, buffer.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ModelRenderer::cleanUp()
{
	for (auto& buffer : models) {
		buffer.cleanUp();
	}
	models.clear();
}
