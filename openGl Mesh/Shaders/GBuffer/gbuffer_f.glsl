#version 440 core

layout(location = 0) out vec4 albedoPos;
layout(location = 1) out vec3 normals;

uniform float numBlocks;

// out vec4 color;

flat in uint blockColourIndex;
in vec3 normal;
in vec3 fragPos;

void main()
{
    // normalises block index such that it can later be multiplyed back into an int
    albedoPos = vec4(fragPos, float(blockColourIndex - 1u) / numBlocks);
    normals = normal;
    normals = vec3(gl_FragCoord.z);
}