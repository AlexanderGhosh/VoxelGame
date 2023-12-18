#version 440 core

layout(location = 0) in uint data;

uniform float voxelSize;
uniform vec3 chunkPosition;

out VS_OUT {
    uint cubeType;
    uint matIndex;
    mat4 model;
} vs_out;

void main() {
    uint z = data & 0x0000000f;
    uint x = (data & 0x000000f0) >> 4;
    uint y = (data & 0x0000ff00) >> 8;
    uint cubeType_ = (data & 0x00ff0000) >> 16;
    uint matIndex = (data & 0xff000000) >> 24;
    vec3 worldPos_ = vec3(float(x), float(y), float(z));

    worldPos_ += chunkPosition;
    
    mat4 model = mat4(voxelSize);
    model[3][0] = worldPos_.x * voxelSize;
    model[3][1] = worldPos_.y * voxelSize;
    model[3][2] = worldPos_.z * voxelSize;
    vs_out.model = model;

    gl_Position.xyz = worldPos_;

    vs_out.cubeType = cubeType_;
    vs_out.matIndex = matIndex;
}