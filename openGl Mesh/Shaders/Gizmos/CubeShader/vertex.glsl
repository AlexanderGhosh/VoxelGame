#version 460 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform Matrices
{
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
    float gamma;
};

out vec3 f_col;

uniform mat4 model;
uniform vec3 colour;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1);
    f_col = colour;
}