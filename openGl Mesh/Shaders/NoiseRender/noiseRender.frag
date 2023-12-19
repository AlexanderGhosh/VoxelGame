#version 440 core

layout(location = 0) out vec3 _albedo;
layout(location = 1) out vec3 _fragPos;
layout(location = 2) out vec3 _normal;

struct Material{
    vec4 albedo1;
    vec4 albedo2;
};

layout (std140, binding = 1) uniform Mats
{
    Material[8] materials;
};

uniform float numBlocks;

flat in uint blockColourIndex;
in vec3 normal;
in vec3 worldPos;
in vec4 viewFragPos_;
flat in vec2 rndSeed;

float rand(vec2 seed);
void main()
{
    uint y = uint(worldPos.y);
    Material mat = materials[blockColourIndex];
    if(y > 38u) {
        mat = materials[6];
    }

    float rndValue = rand(rndSeed);
    _albedo = mix(mat.albedo1, mat.albedo2, rndValue).xyz;
    _fragPos = viewFragPos_.xyz; // view space space
    _normal = normal;
}
float rand(vec2 seed){
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}