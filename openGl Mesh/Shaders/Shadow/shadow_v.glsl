#version 440 core

layout(location = 0) in vec3 worldPos_;
layout(location = 1) in uint cubeType_;
layout(location = 2) in uint blockColourIndex_;

uniform float voxelSize;

out VS_OUT {
    uint cubeType;
    uint blockColourIndex;
    mat4 m;
} vs_out;

void main() {
    mat4 m = mat4(voxelSize);
    m[3][0] = worldPos_.x * voxelSize;
    m[3][1] = worldPos_.y * voxelSize;
    m[3][2] = worldPos_.z * voxelSize;
    vs_out.m = m;

    vs_out.cubeType = cubeType_;
    vs_out.blockColourIndex = blockColourIndex_;
}