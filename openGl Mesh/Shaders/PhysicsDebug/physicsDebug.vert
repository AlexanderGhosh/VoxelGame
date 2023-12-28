#version 460 core
layout(location = 0) in vec3 pos;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};
uniform vec3 colour;

out vec3 f_col;

void main() {
    gl_Position = projection * view * vec4(pos, 1);
    f_col = colour;
}