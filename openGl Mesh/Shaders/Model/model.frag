#version 440 core

layout(location = 0) out vec4 frag;

void main() {
    frag = vec4(vec3(0.4), 1);
}