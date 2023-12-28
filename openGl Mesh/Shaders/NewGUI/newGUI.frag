#version 460 core

layout (location = 0) out vec4 fragColour;

in vec4 o_colour;

void main()
{    
    fragColour = o_colour;
}