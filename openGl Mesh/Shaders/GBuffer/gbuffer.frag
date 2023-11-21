#version 440 core

layout(location = 0) out vec4 albedoPos;
layout(location = 1) out vec4 normalRnd;
layout(location = 2) out vec4 worldPos;

uniform float numBlocks;

flat in uint blockColourIndex;
in vec3 normal;
in vec3 fragPosView;
in vec4 fragPosModel;
flat in vec2 rndSeed;

float rand(vec2 seed);
void main()
{
    // normalises block index such that it can later be multiplyed back into an int
    albedoPos = vec4(fragPosView, blockColourIndex);
    normalRnd.xyz = normal;
    normalRnd.w = rand(rndSeed);
    worldPos = fragPosModel;
}
float rand(vec2 seed){
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}