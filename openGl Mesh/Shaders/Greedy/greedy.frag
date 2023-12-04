#version 460 core

layout(location = 0) out vec3 _albedo;
layout(location = 1) out vec3 _fragPos;
layout(location = 2) out vec3 _normal;

struct Material{
    vec4 albedo1;
    vec4 albedo2;
};

layout (std140, binding = 1) uniform Mats
{
    Material[6] materials;
};

flat in uint matIdx;
in vec3 normal;
in vec3 fragPos_;
// flat in vec2 rndSeed;

float rand(vec2 seed);
void main()
{
    vec2 rndSeed = vec2(0, 1);
    float rndValue = rand(rndSeed);
    Material mat = materials[matIdx];
    _albedo = mix(mat.albedo1, mat.albedo2, rndValue).xyz;
    _fragPos = fragPos_; // view space space
    _normal = normal;
}
float rand(vec2 seed){
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}