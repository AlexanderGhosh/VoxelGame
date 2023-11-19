#version 460 core
layout (points) in;
layout (line_strip, max_vertices = 125) out;

in VS_out {
    vec3 colour;
    mat4 translation;
} gs_in[];

out vec3 f_col;

const int resolution = 100;
uniform mat4 rotation;
uniform mat4 pv;
uniform float radius;
uniform vec3 colour;

float PI_1_5 = 3.1415926 * 2 / resolution;

void main()
{
    f_col = colour;

    for (int i = 0; i <= resolution + 1; i++) {
        // Angle between each side in radians
        float ang = PI_1_5 * i;

        // Offset from center of point (0.3 to accomodate for aspect ratio)
        vec4 offset = pv * rotation * vec4(cos(ang), 0, -sin(ang), 0) * radius;
        gl_Position = gl_in[0].gl_Position + offset;

        EmitVertex();
    }

    EndPrimitive();
}