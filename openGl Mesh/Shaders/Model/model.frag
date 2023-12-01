#version 440 core

layout(location = 0) out vec4 _fragPos;
layout(location = 1) out vec4 _normalRnd;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

void main() {
    _fragPos.xyz = fragPos;
    _fragPos.w = 6;
    _normalRnd.xyz = normal;
    _normalRnd.w = fragPos.x + fragPos.y + fragPos.z;
}