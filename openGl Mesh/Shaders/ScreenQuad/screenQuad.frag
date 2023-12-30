#version 440 core

layout(location = 0) out vec4 frag;

in vec2 texCoords;

uniform sampler2D screen;

void main() {
    vec3 colorWorld = texture(screen, texCoords).rgb;
    frag = vec4(colorWorld, 1);
}