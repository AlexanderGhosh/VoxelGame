#include "ModelRenderer.h"
#include <glad/glad.h>
#include "../IndexedBuffer.h"

ModelRenderer::ModelRenderer() : buffers()
{
}

ModelRenderer::~ModelRenderer()
{
	cleanUp();
}

void ModelRenderer::add(IndexedBuffer& buffer)
{
	buffers.push_back(buffer);
}

void ModelRenderer::render(Shader* shader)
{
	for (auto& buffer : buffers) {
		buffer.bind();
		glDrawElements(GL_TRIANGLES, buffer.size(), GL_UNSIGNED_INT, nullptr);
	}
}

void ModelRenderer::cleanUp()
{
	for (auto& buffer : buffers) {
		buffer.cleanUp();
	}
	buffers.clear();
}
