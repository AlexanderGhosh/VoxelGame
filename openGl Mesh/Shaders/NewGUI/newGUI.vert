#version 460 core
layout (location = 0) in vec2 screenPos;
layout (location = 1) in vec4 colour;

uniform mat4 projection; // GUI projection not the camera one

uniform vec2 position;
uniform vec2 dimentions;

out vec4 o_colour;

void main()
{
    o_colour = colour;
    vec2 pos = screenPos * dimentions; // scale from unit to the propper pixel size
    pos += position; // translate to pixel space positoin
    vec4 pixelPos = vec4(pos, 0, 1);
    gl_Position = projection * pixelPos;
}  