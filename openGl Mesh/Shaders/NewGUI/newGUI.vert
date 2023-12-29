#version 460 core
layout (location = 0) in vec2 screenPos;
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
    vec2 pos = screenPos * 2.0 - 1.0;
    gl_Position = vec4(pos, 0, 1);
}  