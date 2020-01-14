#pragma once
#include "../../Mesh.h"
#include "../../Buffer.h"

#define RENDER_DISTANCE 2
#define CHUNK_SIZE 16
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

#define GRAVITY 9.81
#define AIR_RESISTANCE 2

using namespace Mesh;

struct FACES {
	static FaceMesh BACK;
	static FaceMesh FRONT;
	static FaceMesh LEFT;
	static FaceMesh RIGHT;
	static FaceMesh BOTTOM;
	static FaceMesh TOP;
};
