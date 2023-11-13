#include "Model.h"
#include "Mesh.h"
#include "../../IndexedBuffer.h"

void Model::setUp(const std::list<Mesh>& meshes)
{
	meshBuffers.reserve(meshes.size());
	for (const Mesh& mesh : meshes) {
		meshBuffers.emplace_back(mesh);
	}
}

void Model::cleanUp()
{
	for (IndexedBuffer& buffer : meshBuffers) {
		buffer.cleanUp();
	}
	meshBuffers.clear();
}
