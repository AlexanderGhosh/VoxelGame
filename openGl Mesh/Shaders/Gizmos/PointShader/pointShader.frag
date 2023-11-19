#version 460 core

layout(location = 0) out vec4 frag;

in vec3 f_col;

void main() {
     frag = vec4(f_col, 1);
}