#version 460 core
layout (location = 0) in vec4 vertex;

out vec2 texCoords;
uniform mat4 projection; // not the camera's projection but an orthographic one of the screen size

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    texCoords = vertex.zw;
} 