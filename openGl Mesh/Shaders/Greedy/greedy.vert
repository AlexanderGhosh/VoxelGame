#version 460 core

layout(location = 0) in vec3 _vert0;
layout(location = 1) in vec3 _vert1;
layout(location = 2) in vec3 _vert2;
layout(location = 3) in vec3 _vert3;
layout(location = 4) in vec3 _normal;
layout(location = 5) in uint _material;

uniform float voxelSize;
uniform vec3 chunkPosition;

out vec3 vert0;
out vec3 vert1;
out vec3 vert2;
out vec3 vert3;
out vec3 normal;
flat out uint materialIndex;


void main() {
    vert0 = (_vert0 + chunkPosition) * voxelSize;
    vert1 = (_vert1 + chunkPosition) * voxelSize;
    vert2 = (_vert2 + chunkPosition) * voxelSize;
    vert3 = (_vert3 + chunkPosition) * voxelSize;
    normal = _normal;

    materialIndex = _material;
}