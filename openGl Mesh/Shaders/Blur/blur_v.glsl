#version 440 core

layout(location = 0) in vec2 vertex;

out vec2 texCoords;

void main() {
    gl_Position = vec4(vertex, 0, 1);
    texCoords = (vertex + 1) * .5;
}