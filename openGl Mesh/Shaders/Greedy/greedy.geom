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
in vec3[] normal_;
flat in uint[] materialIndex;

flat out uint matIndex;
flat out vec2 rndSeed;
out vec4 viewPos;
out vec3 worldPos;
out vec3 normal;

void main() {
    const vec3 pos0 = vert0[0];
    const vec3 pos1 = vert1[0];
    const vec3 pos2 = vert2[0];
    const vec3 pos3 = vert3[0];

    const vec3 avgPos = (pos0 + pos1 + pos2 + pos3) * 0.25;

    normal = normal_[0];
    matIndex = materialIndex[0];

    worldPos = pos0;
    viewPos = view * vec4(worldPos, 1);
    rndSeed = avgPos.xy + avgPos.yz;
    gl_Position = projection * viewPos;
    EmitVertex();

    worldPos = pos1;
    viewPos = view * vec4(worldPos, 1);
    rndSeed = avgPos.xy + avgPos.yz;
    gl_Position = projection * viewPos;
    EmitVertex();

    worldPos = pos2;
    viewPos = view * vec4(worldPos, 1);
    rndSeed = avgPos.xy + avgPos.yz;
    gl_Position = projection * viewPos;
    EmitVertex();

    worldPos = pos3;
    viewPos = view * vec4(worldPos, 1);
    rndSeed = avgPos.xy + avgPos.yz;
    gl_Position = projection * viewPos;
    EmitVertex();


    EndPrimitive();
}