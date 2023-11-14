#version 440 core

layout(location = 0) out vec4 frag;

uniform sampler2D albedoPos;
uniform sampler2D normals;
uniform float numBlocks;

in vec2 texCoords;

const vec4[] blockColours = vec4[] (
    vec4(.5, 0, .5, 1),
    vec4(0, 0.4, 0.1, 1),
    vec4(.5, .25, .25, 1),
    vec4(.65, .65, .65, 1),
    vec4(0, 0.6, 1, 0.5),
    vec4(.5, .25, .25, 1),
    vec4(1, 0.9, 0.7, 1), // sand
    vec4(1, 0.9, 0.7, 1)
);
float rand(vec2 co);
void main() {
    vec4 albedoPosSample = texture(albedoPos, texCoords);
    float colorIndexNormalised = albedoPosSample.w;
    uint colourIndex = uint(colorIndexNormalised * numBlocks);

    vec3 pos = albedoPosSample.xyz;
    vec3 normal = texture(normals, texCoords).xyz;
    vec3 albedo = (blockColours[colourIndex]).rgb;
    // only outputs the albedo
    frag = vec4(albedo, 1);
}
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}