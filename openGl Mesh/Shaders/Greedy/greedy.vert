#version 440 core

layout(location = 0) in vec3 _vert;
layout(location = 1) in uint _material;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};

uniform float voxelSize;
uniform vec3 chunkPosition;

flat out uint materialIndex;
out vec3 fragPos_;
out vec3 normal;

// gl_Postion hold the max coord

void main() {
    normal = vec3(0, 1, 0);
    vec3 pos = (chunkPosition + _vert) * voxelSize;
    fragPos_ = (view * vec4(pos, 1.0)).xyz;
    gl_Position = projection * vec4(fragPos_, 1.0);
    materialIndex = _material;
}