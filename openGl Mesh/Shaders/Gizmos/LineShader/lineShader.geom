#version 460 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;

out vec3 f_col;
uniform vec3 start;
uniform vec3 end;
uniform vec3 colour;
uniform mat4 pv;

void main()
{
    f_col = colour;

    gl_Position = pv * (vec4(start, 1));
    EmitVertex();
    gl_Position = pv * (vec4(end, 1));
    EmitVertex();

    EndPrimitive();
}