#include "Model.h"
#include "Mesh.h"
#include "../../IndexedBuffer.h"
#include <glad/glad.h>

void Model::render() const
{
	for (auto& _buffer : _meshBuffers) {
		_buffer.bind();
		glDrawElements(GL_TRIANGLES, _buffer.size(), GL_UNSIGNED_INT, nullptr);
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
	for (IndexedBuffer& _buffer : _meshBuffers) {
		_buffer.cleanUp();
	}
	_meshBuffers.clear();
}
