#version 440 core

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texC;

out vec3 normal;
out vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(vert, 1);
    normal = norm;
    texCoords = texC;
}