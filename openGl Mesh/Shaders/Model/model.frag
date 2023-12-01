#version 440 core

layout(location = 0) out vec3 _albedo;
layout(location = 1) out vec3 _fragPos;
layout(location = 2) out vec3 _normal;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

void main() {
    _albedo = normal;
    _fragPos = fragPos;
    _normal = normal;
}