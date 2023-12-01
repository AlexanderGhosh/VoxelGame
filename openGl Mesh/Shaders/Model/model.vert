#version 440 core

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texC;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};

out vec3 normal;
out vec2 texCoords;


void main() {
    gl_Position = projection * view * vec4(vert, 1);
    normal = norm;
    texCoords = texC;
}