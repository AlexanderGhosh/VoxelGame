#version 460 core

out vec4 FragColor;

in vec3 f_col;

void main() {
     FragColor = vec4(f_col, 1);
}