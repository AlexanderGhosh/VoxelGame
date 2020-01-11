#pragma once
#include "../../Mesh.h"
#include "../../Buffer.h"

#define CHUNK_SIZE 32
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE
#pragma region FACES
// 3D texture
using namespace Mesh;
struct FACES {
	static FaceMesh BACK;
	static FaceMesh FRONT;
	static FaceMesh LEFT;
	static FaceMesh RIGHT;
	static FaceMesh BOTTOM;
	static FaceMesh TOP;
};
#pragma endregion