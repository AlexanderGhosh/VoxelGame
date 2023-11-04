#version 440 core

layout(location = 0) in vec3 worldPos_;
layout(location = 1) in uint cubeType_;
layout(location = 2) in uint blockColourIndex_;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float voxelSize;

out VS_OUT {
    uint cubeType;
    uint blockColourIndex;
    mat4 vp;
    mat4 m;
} vs_out;

void main() {
    mat4 m = mat4(voxelSize);
    m[3][0] = worldPos_.x * voxelSize - 1;
    m[3][1] = worldPos_.y * voxelSize - 1;
    m[3][2] = worldPos_.z * voxelSize - 1;
    vs_out.m = m;

    vs_out.vp = projection * view;

    gl_Position.rgb = worldPos_;

    vs_out.cubeType = cubeType_;
    vs_out.blockColourIndex = blockColourIndex_;
}