#version 460 core
layout(location = 0) in vec3 pos;


uniform mat4 pv;

out vec3 f_col;

uniform vec3 position;
uniform vec3 colour;

void main() {
    gl_Position = pv * vec4(position, 1);

    f_col = colour;
}