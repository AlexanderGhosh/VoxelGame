#include "Model.h"
#include "Mesh.h"
#include "../../IndexedBuffer.h"
#include <glad/glad.h>

void Model::render() const
{
	for (auto& buffer_ : _meshBuffers) {
		buffer_.bind();
		glDrawElements(GL_TRIANGLES, buffer_.size(), GL_UNSIGNED_INT, nullptr);
	}
}

void Model::setUp(const std::list<Mesh>& meshes)
{
	_meshBuffers.reserve(meshes.size());
	for (const Mesh& mesh : meshes) {
		_meshBuffers.emplace_back(mesh);
	}
}

void Model::cleanUp()
{
	for (IndexedBuffer& buffer_ : _meshBuffers) {
		buffer_.cleanUp();
	}
	_meshBuffers.clear();
}
