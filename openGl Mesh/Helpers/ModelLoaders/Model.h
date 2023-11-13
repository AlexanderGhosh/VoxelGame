#pragma once
#include <vector>
#include <list>
#include "../../IndexedBuffer.h"

class Mesh;

class Model {
public:
	std::vector<IndexedBuffer> meshBuffers;
	void setUp(const std::list<Mesh>& meshes);
	void cleanUp();
};
