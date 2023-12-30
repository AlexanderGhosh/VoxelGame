#version 460 core
layout (location = 0) in vec2 screenPos;

uniform mat4 projection; // GUI projection not the camera one

uniform float borderSize;
uniform vec2 position;
uniform vec2 dimentions;

void main()
{
    vec2 pos = screenPos * dimentions; // scale from unit to the propper pixel size
    // translate to pixel space position (with 0, 0, at bottom left)
    pos += position;

    vec4 pixelPos = vec4(pos, 0, 1);
    gl_Position = projection * pixelPos;
    // gl_Position.y *= -1;
}  