#version 440 core

layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec4 normalRnd;

uniform float numBlocks;

flat in uint blockColourIndex;
in vec3 normal;
in vec4 fragPos_;
flat in vec2 rndSeed;

float rand(vec2 seed);
void main()
{
    // normalises block index such that it can later be multiplyed back into an int
    fragPos.xyz = fragPos_.xyz; // world space
    fragPos.w = float(blockColourIndex);
    normalRnd.xyz = normal;
    normalRnd.w = rand(rndSeed);
}
float rand(vec2 seed){
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}