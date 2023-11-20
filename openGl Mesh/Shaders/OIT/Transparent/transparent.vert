#version 440 core

layout(location = 0) in uint data;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float voxelSize;
uniform vec3 chunkPosition;

out VS_OUT {
    uint cubeType;
    uint blockColourIndex;
    mat4 vp;
    mat4 m;
} vs_out;

void main() {
    uint z = data & 0x0000000f;
    uint x = (data & 0x000000f0) >> 4;
    uint y = (data & 0x0000ff00) >> 8;
    uint cubeType_ = (data & 0x00ff0000) >> 16;
    uint blockColourIndex_ = (data & 0xff000000) >> 24;
    vec3 worldPos_ = vec3(float(x), float(y), float(z));

    worldPos_ += chunkPosition;
    worldPos_.y += voxelSize == 1 ? 0 : 20.0 / (1.0 - voxelSize);
    mat4 m = mat4(1);
    m[3][0] = worldPos_.x * voxelSize;
    m[3][1] = worldPos_.y * voxelSize;
    m[3][2] = worldPos_.z * voxelSize;
    vs_out.m = m;

    vs_out.vp = projection * view;

    gl_Position.rgb = worldPos_;

    vs_out.cubeType = cubeType_;
    vs_out.blockColourIndex = blockColourIndex_;
}