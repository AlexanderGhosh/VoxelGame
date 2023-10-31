#version 440 core

layout(location = 0) out vec4 frag;

in vec2 texCoords;

layout(binding = 0) uniform sampler2D screen;
layout(binding = 1) uniform sampler2D gui;

void main() {
    vec3 colorWorld = texture(screen, texCoords).rgb;
    vec4 GUI = texture(screen, texCoords);
    vec3 colorGUI = texture(gui, texCoords).rgb;
    float mixer = texture(gui, texCoords).a;

    vec3 color = mix(colorWorld, colorGUI, mixer);

    frag = vec4(color, 1);
}