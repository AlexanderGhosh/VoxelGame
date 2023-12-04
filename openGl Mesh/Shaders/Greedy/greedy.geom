#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};

in vec3[] vert0;
in vec3[] vert1;
in vec3[] vert2;
in vec3[] vert3;
in vec3[] normal;
flat in uint[] materialIndex;

out vec3 norm;
out vec4 fragPos_;
flat out uint matIdx;

void main() {
    vec3 pos0 = vert0[0];
    vec3 pos1 = vert1[0];
    vec3 pos2 = vert2[0];
    vec3 pos3 = vert3[0];
    norm = normal[0];
    matIdx = materialIndex[0];

    fragPos_ = view * vec4(pos0, 1);
    gl_Position = projection * fragPos_;
    EmitVertex();

    fragPos_ = view * vec4(pos1, 1);
    gl_Position = projection * fragPos_;
    EmitVertex();

    fragPos_ = view * vec4(pos2, 1);
    gl_Position = projection * fragPos_;
    EmitVertex();

    fragPos_ = view * vec4(pos3, 1);
    gl_Position = projection * fragPos_;
    EmitVertex();


    EndPrimitive();
}