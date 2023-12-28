#version 460 core
layout (location = 0) in vec2 viewPos;
layout (location = 1) in vec4 colour;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};

out vec4 o_colour;

void main()
{
    o_colour = colour;
    gl_Position = projection * vec4(viewPos, 0, 1);
}  