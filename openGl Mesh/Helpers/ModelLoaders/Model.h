#pragma once
#include <vector>
#include <list>
#include "../../IndexedBuffer.h"

class Mesh;
class Shader;

class Model {
private:
	std::vector<IndexedBuffer> _meshBuffers;
public:
	void render() const;
	void setUp(const std::list<Mesh>& meshes);
	void cleanUp();
};
