#version 460 core
layout(location = 0) in vec3 pos;
layout(location = 1) in uint colour;

uniform mat4 pv;

out vec3 f_col;

void main() {
    gl_Position = pv * vec4(pos, 1);
    f_col = vec3(0, 0.7, 0);
}