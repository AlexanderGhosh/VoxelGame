#version 440 core

layout(location = 0) in vec2 vertices;

void main() {
    gl_Position = vec4(vertices, 0, 1);
}