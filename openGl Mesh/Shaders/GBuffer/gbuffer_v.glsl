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
    mat4 view;
    mat4 proj;
    mat4 m;
} vs_out;

void main() {
    /*
    uint x = (data & 0xf0000000) >> 28;
    uint z = (data & 0x0f000000) >> 24;
    uint y = (data & 0x00ff0000) >> 16;
    uint cubeType_ = data & 0x0000ff00 >> 8;
    uint blockColourIndex_ = data & 0x000000ff;
    vec3 worldPos_ = vec3(float(x), float(y), float(z));
    */
    uint z = data & 0x0000000f;
    uint x = (data & 0x000000f0) >> 4;
    uint y = (data & 0x0000ff00) >> 8;
    uint cubeType_ = (data & 0x00ff0000) >> 16;
    uint blockColourIndex_ = (data & 0xff000000) >> 24;
    vec3 worldPos_ = vec3(float(x), float(y), float(z));

    worldPos_ += chunkPosition;
    mat4 m = mat4(voxelSize);
    m[3][0] = worldPos_.x * voxelSize;
    m[3][1] = worldPos_.y * voxelSize;
    m[3][2] = worldPos_.z * voxelSize;
    vs_out.m = m;

    // vs_out.vp = projection * view;
    vs_out.view = view;
    vs_out.proj = projection;

    gl_Position.xyz = worldPos_;

    vs_out.cubeType = cubeType_;
    vs_out.blockColourIndex = blockColourIndex_;
}